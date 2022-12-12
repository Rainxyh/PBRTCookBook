

#include "Integrator/Integrator.h"
#include "Sampler/Sampler.h"

#include "Core/Spectrum.h"
#include "Core/interaction.h"
#include "Core/Scene.h"
#include "Core/FrameBuffer.h"

#include "Material/Reflection.h"

#include <omp.h>

namespace Feimos
{

	void SamplerIntegrator::Render(const Scene &scene, double &timeConsume)
	{

		omp_set_num_threads(32);		//�����̵߳ĸ���
		double start = omp_get_wtime(); //��ȡ��ʼʱ��

		// ��Ⱦ֡����1
		m_FrameBuffer->renderCountIncrease();

		Feimos::Point3f Light(10.0, 10.0, -10.0);

// #pragma omp parallel for
		for (int i = 0; i < pixelBounds.pMax.x; i++)
		{
			for (int j = 0; j < pixelBounds.pMax.y; j++)
			{

				float u = float(i + getClockRandom()) / float(pixelBounds.pMax.x);
				float v = float(j + getClockRandom()) / float(pixelBounds.pMax.y);
				int offset = (pixelBounds.pMax.x * j + i);

				std::unique_ptr<Feimos::Sampler> pixel_sampler = sampler->Clone(offset);
				Feimos::Point2i pixel(i, j);
				pixel_sampler->StartPixel(pixel);

				Feimos::CameraSample cs;
				cs = pixel_sampler->GetCameraSample(pixel);
				Feimos::Ray r;
				camera->GenerateRay(cs, &r);

				Feimos::SurfaceInteraction isect;
				Feimos::Spectrum colObj(.0f);
				if (scene.Intersect(r, &isect))
				{
					//����ɢ��
					isect.ComputeScatteringFunctions(r);
					// ���������������˵��wo����Ӱ�����Ľ��
					Vector3f wo = isect.wo;
					Vector3f wi = Normalize(Light - isect.p);
					Spectrum f = isect.bsdf->f(wo, wi);
					float pdf = isect.bsdf->Pdf(wo, wi);

					Vector3f wh = Normalize(-r.d + wi);
					float La, Ld, Ls;
					La = .2f;
					Ld = Dot(wi, isect.n) > 0.f ? Dot(wi, isect.n) : 0.f;
					Ls = Dot(wh, isect.n) > 0.f ? Dot(wh, isect.n) : 0.f;
					Ls = pow(Ls, 4);
					float Li = 1.5f * (La + .2f * Ld + 1.7f * Ls);

					// ����3.0��������Ϊ�˲���ͼ�����
					colObj += Li * pdf * f * 3.0f;
				}

				m_FrameBuffer->update_f_u_c(i, j, 0, colObj[0]);
				m_FrameBuffer->update_f_u_c(i, j, 1, colObj[1]);
				m_FrameBuffer->update_f_u_c(i, j, 2, colObj[2]);
				m_FrameBuffer->set_uc(i, pixelBounds.pMax.y - j - 1, 3, 255);
			}
		}

		// ���㲢��ʾʱ��
		double end = omp_get_wtime();
		timeConsume = end - start;
	}

};

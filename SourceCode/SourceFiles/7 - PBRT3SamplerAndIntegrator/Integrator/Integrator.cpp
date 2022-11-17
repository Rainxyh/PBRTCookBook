

#include "Integrator/Integrator.h"
#include "Sampler/Sampler.h"
#include "Core/Spectrum.h"
#include "Core/interaction.h"
#include "Core/Scene.h"
#include "Core/FrameBuffer.h"
#include <omp.h>

namespace Feimos {

void SamplerIntegrator::Render(const Scene &scene, double &timeConsume) {

	omp_set_num_threads(32); //设置线程的个数
	double start = omp_get_wtime();//获取起始时间  

	// 渲染帧数加1
	m_FrameBuffer->renderCountIncrease();

	Feimos::Point3f Light(10.0, 10.0, -10.0);

#pragma omp parallel for
	for (int i = 0; i < pixelBounds.pMax.x; i++) {
		for (int j = 0; j < pixelBounds.pMax.y; j++) {

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
			Feimos::Spectrum colObj(0.0f); colObj[0] = 1.0f; colObj[1] = 1.0f;
			if (scene.Intersect(r, &isect)) {
				Vector3f LightNorm = Feimos::Normalize(Light - isect.p);
				Vector3f viewInv = -r.d;
				//半程向量
				Vector3f H = Normalize(viewInv + LightNorm); 
				//高光
				float Ls = Dot(H, isect.n); Ls = (Ls > 0.0f) ? Ls : 0.0f;
				Ls = pow(Ls, 32);
				//漫反射光
				float Ld = Dot(LightNorm, isect.n); Ld = (Ld > 0.0f) ? Ld : 0.0f;
				float Li = (0.2 + 0.2 * Ld + 0.6 * Ls);
				colObj = std::abs(Li) * colObj; //取绝对值，防止出现负值
			}

			m_FrameBuffer->update_f_u_c(i, j, 0, colObj[0]);
			m_FrameBuffer->update_f_u_c(i, j, 1, colObj[1]);
			m_FrameBuffer->update_f_u_c(i, j, 2, colObj[2]);
			m_FrameBuffer->set_uc(i, pixelBounds.pMax.y - j - 1, 3, 255);
		}
	}

	// 计算并显示时间
	double end = omp_get_wtime();
	timeConsume = end - start;


	
}



};




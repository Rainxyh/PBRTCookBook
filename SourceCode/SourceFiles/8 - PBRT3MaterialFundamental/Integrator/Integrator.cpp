#pragma once

#include "Integrator\Integrator.h"
#include "Sampler\Sampler.h"

#include "Core\Spectrum.h"
#include "Core\interaction.h"
#include "Core\Scene.h"
#include "Core\frameBuffer.h"

#include "Material\Reflection.h"

#include <omp.h>

namespace Feimos {

void SamplerIntegrator::Render(const Scene &scene, double &timeConsume) {

	omp_set_num_threads(20); //设置线程的个数
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
			Feimos::Spectrum colObj(.0f); 
			if (scene.Intersect(r, &isect)) {
				//计算散射
				isect.ComputeScatteringFunctions(r);
				// 对于漫反射材质来说，wo不会影响后面的结果
				Vector3f wo = isect.wo;
				Vector3f LightNorm = Light - isect.p;
				LightNorm = Normalize(LightNorm);
				Vector3f wi = LightNorm;
				Spectrum f = isect.bsdf->f(wo, wi);
				float pdf = isect.bsdf->Pdf(wo, wi);
				//乘以3.0的意义是为了不让图像过暗
				colObj += pdf * f * 3.0f;
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




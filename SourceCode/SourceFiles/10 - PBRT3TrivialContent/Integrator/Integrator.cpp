

#include "Integrator/Integrator.h"
#include "Sampler/Sampler.h"

#include "Core/Spectrum.h"
#include "Core/interaction.h"
#include "Core/Scene.h"
#include "Core/FrameBuffer.h"

#include "Material/Reflection.h"

#include "Light/Light.h"

#include <omp.h>

namespace Feimos {

Spectrum SamplerIntegrator::SpecularReflect(
	const Ray &ray, const SurfaceInteraction &isect,
	const Scene &scene, Sampler &sampler, int depth) const {
	// Compute specular reflection direction _wi_ and BSDF value
	Vector3f wo = isect.wo, wi;
	float pdf;
	BxDFType type = BxDFType(BSDF_REFLECTION | BSDF_SPECULAR);
	Spectrum f = isect.bsdf->Sample_f(wo, &wi, sampler.Get2D(), &pdf, type);

	// Return contribution of specular reflection
	const Normal3f &ns = isect.shading.n;

	if (wi.HasNaNs()) return 1.0f;

	if (pdf > 0.f && !f.IsBlack() && AbsDot(wi, ns) != 0.f) {
		// Compute ray differential _rd_ for specular reflection
		Ray rd = isect.SpawnRay(wi);
		return  f *Li(rd, scene, sampler, depth + 1) *AbsDot(wi, ns) / pdf;
	}
	else
		return Spectrum(0.f);
}


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

			std::unique_ptr<Feimos::Sampler> sampler_c = sampler->Clone(offset);
			Feimos::Point2i pixel(i, j);
			sampler_c->StartPixel(pixel);

			Feimos::CameraSample cs;
			cs = sampler_c->GetCameraSample(pixel);
			Feimos::Ray ray;
			camera->GenerateRay(cs, &ray);

			
			Feimos::Spectrum colObj = Li(ray, scene, *sampler_c, 0);

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


Spectrum SamplerIntegrator::Li(const Ray &ray, const Scene &scene,
	Sampler &sampler, int depth) const {

	Feimos::SurfaceInteraction isect;

	Feimos::Spectrum colObj;
	if (scene.Intersect(ray, &isect)) {
		for (int count = 0; count < scene.lights.size(); count++) {
			VisibilityTester vist;
			Vector3f wi;
			Interaction p1;
			float pdf_light;
			Spectrum Li = scene.lights[count]->Sample_Li(isect, sampler.Get2D(), &wi, &pdf_light, &vist);

			if (vist.Unoccluded(scene)) {
				//计算散射
				isect.ComputeScatteringFunctions(ray);
				// 对于漫反射材质来说，wo不会影响后面的结果
				Vector3f wo = isect.wo;
				Spectrum f = isect.bsdf->f(wo, wi);
				float pdf_scattering = isect.bsdf->Pdf(wo, wi);
				colObj += Li * pdf_scattering * f *3.0f / pdf_light;
			}
		}
	}

	return colObj;
}




};




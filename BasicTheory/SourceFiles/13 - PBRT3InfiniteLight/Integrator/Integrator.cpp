

#include "Integrator/Integrator.h"
#include "Sampler/Sampler.h"
#include "Sampler/Sampling.h"

#include "Core/Spectrum.h"
#include "Core/interaction.h"
#include "Core/Scene.h"
#include "Core/FrameBuffer.h"

#include "Material/Reflection.h"

#include "Light/Light.h"

#include <omp.h>

namespace Feimos
{

	// Integrator Utility Functions
	Spectrum UniformSampleAllLights(const Interaction &it, const Scene &scene, Sampler &sampler,
									const std::vector<int> &nLightSamples, bool handleMedia)
	{
		Spectrum L(0.f);
		for (size_t j = 0; j < scene.lights.size(); ++j)
		{
			// Accumulate contribution of _j_th light to _L_
			const std::shared_ptr<Light> &light = scene.lights[j];
			int nSamples = nLightSamples[j];
			const Point2f *uLightArray = sampler.Get2DArray(nSamples);
			const Point2f *uScatteringArray = sampler.Get2DArray(nSamples);
			if (!uLightArray || !uScatteringArray)
			{
				// Use a single sample for illumination from _light_
				Point2f uLight = sampler.Get2D();
				Point2f uScattering = sampler.Get2D();
				L += EstimateDirect(it, uScattering, *light, uLight, scene, sampler, handleMedia);
			}
			else
			{
				// Estimate direct lighting using sample arrays
				Spectrum Ld(0.f);
				for (int k = 0; k < nSamples; ++k)
					Ld += EstimateDirect(it, uScatteringArray[k], *light,
										 uLightArray[k], scene, sampler, handleMedia);
				L += Ld / nSamples;
			}
		}
		return L;
	}

	Spectrum UniformSampleOneLight(const Interaction &it, const Scene &scene, Sampler &sampler,
								   bool handleMedia, const Distribution1D *lightDistrib)
	{
		// Randomly choose a single light to sample, _light_
		int nLights = int(scene.lights.size());
		if (nLights == 0)
			return Spectrum(0.f);
		int lightNum;
		float lightPdf;
		if (lightDistrib)
		{
			lightNum = lightDistrib->SampleDiscrete(sampler.Get1D(), &lightPdf);
			if (lightPdf == 0)
				return Spectrum(0.f);
		}
		else
		{
			lightNum = std::min((int)(sampler.Get1D() * nLights), nLights - 1);
			lightPdf = float(1) / nLights;
		}
		const std::shared_ptr<Light> &light = scene.lights[lightNum];
		Point2f uLight = sampler.Get2D();
		Point2f uScattering = sampler.Get2D();
		return EstimateDirect(it, uScattering, *light, uLight,
							  scene, sampler, handleMedia) /
			   lightPdf;
	}

	Spectrum EstimateDirect(const Interaction &it, const Point2f &uScattering, const Light &light,
							const Point2f &uLight, const Scene &scene, Sampler &sampler, bool handleMedia, bool specular)
	{
		BxDFType bsdfFlags =
			specular ? BSDF_ALL : BxDFType(BSDF_ALL & ~BSDF_SPECULAR);
		Spectrum Ld(0.f);
		// Sample light source with multiple importance sampling
		Vector3f wi;

		float lightPdf = 0, scatteringPdf = 0;

		VisibilityTester visibility;
		Spectrum Li = light.Sample_Li(it, uLight, &wi, &lightPdf, &visibility);
		if (lightPdf > 0 && !Li.IsBlack())
		{
			// Compute BSDF or phase function's value for light sample
			Spectrum f;
			if (it.IsSurfaceInteraction())
			{
				// Evaluate BSDF for light sampling strategy
				const SurfaceInteraction &isect = (const SurfaceInteraction &)it;
				f = isect.bsdf->f(isect.wo, wi, bsdfFlags) *
					AbsDot(wi, isect.shading.n);
				scatteringPdf = isect.bsdf->Pdf(isect.wo, wi, bsdfFlags);
			}
			if (!f.IsBlack())
			{
				if (!visibility.Unoccluded(scene))
				{
					Li = Spectrum(0.f);
				}
				// Add light's contribution to reflected radiance
				if (!Li.IsBlack())
				{
					if (IsDeltaLight(light.flags))
						Ld += f * Li / lightPdf;
					else
					{
						float weight = PowerHeuristic(1, lightPdf, 1, scatteringPdf);
						Ld += f * Li * weight / lightPdf;
					}
				}
			}
		}

		// Sample BSDF with multiple importance sampling
		if (!IsDeltaLight(light.flags))
		{
			Spectrum f;
			bool sampledSpecular = false;
			if (it.IsSurfaceInteraction())
			{
				// Sample scattered direction for surface interactions
				BxDFType sampledType;
				const SurfaceInteraction &isect = (const SurfaceInteraction &)it;
				f = isect.bsdf->Sample_f(isect.wo, &wi, uScattering, &scatteringPdf,
										 bsdfFlags, &sampledType);
				f *= AbsDot(wi, isect.shading.n);
				sampledSpecular = (sampledType & BSDF_SPECULAR) != 0;
			}
			if (!f.IsBlack() && scatteringPdf > 0)
			{
				// Account for light contributions along sampled direction _wi_
				float weight = 1;
				if (!sampledSpecular)
				{
					lightPdf = light.Pdf_Li(it, wi);
					if (lightPdf == 0)
						return Ld;
					weight = PowerHeuristic(1, scatteringPdf, 1, lightPdf);
				}
				// Find intersection and compute transmittance
				SurfaceInteraction lightIsect;
				Ray ray = it.SpawnRay(wi);
				bool foundSurfaceInteraction = scene.Intersect(ray, &lightIsect);

				// Add light contribution from material sampling
				Spectrum Li(0.f);
				if (foundSurfaceInteraction)
				{
					if (lightIsect.primitive->GetAreaLight() == &light)
						Li = lightIsect.Le(-wi);
				}
				else
					Li = light.Le(ray);
				if (!Li.IsBlack())
					Ld += f * Li * weight / scatteringPdf;
			}
		}

		return Ld;
	}

	Spectrum SamplerIntegrator::SpecularReflect(
		const Ray &ray, const SurfaceInteraction &isect,
		const Scene &scene, Sampler &sampler, int depth) const
	{
		// Compute specular reflection direction _wi_ and BSDF value
		Vector3f wo = isect.wo, wi;
		float pdf;
		BxDFType type = BxDFType(BSDF_REFLECTION | BSDF_SPECULAR);
		Spectrum f = isect.bsdf->Sample_f(wo, &wi, sampler.Get2D(), &pdf, type);

		// Return contribution of specular reflection
		const Normal3f &ns = isect.shading.n;

		if (wi.HasNaNs())
			return 1.0f;

		if (pdf > 0.f && !f.IsBlack() && AbsDot(wi, ns) != 0.f)
		{
			// Compute ray differential _rd_ for specular reflection
			Ray rd = isect.SpawnRay(wi);
			return f * Li(rd, scene, sampler, depth + 1) * AbsDot(wi, ns) / pdf;
		}
		else
			return Spectrum(0.f);
	}

	void SamplerIntegrator::Render(const Scene &scene, double &timeConsume)
	{

		omp_set_num_threads(32);		//设置线程的个数
		double start = omp_get_wtime(); //获取起始时间

		// 场景预处理
		Preprocess(scene, *sampler);

		// 渲染帧数加1
		m_FrameBuffer->renderCountIncrease();

		Feimos::Point3f Light(10.0, 10.0, -10.0);

#pragma omp parallel for
		for (int i = 0; i < pixelBounds.pMax.x; i++)
		{
			for (int j = 0; j < pixelBounds.pMax.y; j++)
			{

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
#define gamma_correct false
#if gamma_correct
				float xyz[3];
				colObj.ToXYZ(xyz);
				float rgb[3];
				XYZToRGB(xyz, rgb);
// #define TO_BYTE(v) (uint8_t) Clamp(255.f * GammaCorrect(v) + .5f, 0.f, 255.f);
// 				colObj[0] = TO_BYTE(colObj[0]);
// 				colObj[1] = TO_BYTE(colObj[1]);
// 				colObj[2] = TO_BYTE(colObj[2]);
#undef TO_BYTE
#endif
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
								   Sampler &sampler, int depth) const
	{

		Feimos::SurfaceInteraction isect;

		Feimos::Spectrum colObj;
		if (scene.Intersect(ray, &isect))
		{
			for (int count = 0; count < scene.lights.size(); count++)
			{
				VisibilityTester vist;
				Vector3f wi;
				Interaction p1;
				float pdf_light;
				Spectrum Li = scene.lights[count]->Sample_Li(isect, sampler.Get2D(), &wi, &pdf_light, &vist);

				if (vist.Unoccluded(scene))
				{
					//计算散射
					isect.ComputeScatteringFunctions(ray);
					// 对于漫反射材质来说，wo不会影响后面的结果
					Vector3f wo = isect.wo;
					Spectrum f = isect.bsdf->f(wo, wi);
					float pdf_scattering = isect.bsdf->Pdf(wo, wi);
					colObj += Li * pdf_scattering * f * 3.0f / pdf_light;
				}
			}
		}

		return colObj;
	}
};

#include "Light/LightDistrib.h"
#include "Sampler/Sampler.h"
#include "Core/Geometry.h"
#include "Core/interaction.h"
#include "Core/Scene.h"
#include "Light/Light.h"
#include "Material/Reflection.h"
#include "Integrator/VolPathIntegrator.h"
#include "Core/Spectrum.h"

namespace Feimos {

static long long volumeInteractions = 0;
static long long surfaceInteractions = 0;

void VolPathIntegrator::Preprocess(const Scene &scene, Sampler &sampler) {
	lightDistribution =
		CreateLightSampleDistribution(lightSampleStrategy, scene);
}

Spectrum VolPathIntegrator::Li(const RayDifferential &r, const Scene &scene,
	Sampler &sampler, int depth) const {
		
	Spectrum L(0.f), beta(1.f);
	RayDifferential ray(r);
	bool specularBounce = false;
	int bounces;

	// Added after book publication: etaScale tracks the accumulated effect
	// of radiance scaling due to rays passing through refractive
	// boundaries (see the derivation on p. 527 of the third edition). We
	// track this value in order to remove it from beta when we apply
	// Russian roulette; this is worthwhile, since it lets us sometimes
	// avoid terminating refracted rays that are about to be refracted back
	// out of a medium and thus have their beta value increased.
	float etaScale = 1;

	for (bounces = 0;; ++bounces) {
		// Intersect _ray_ with scene and store intersection in _isect_
		SurfaceInteraction isect;
		bool foundIntersection = scene.Intersect(ray, &isect);

		// Sample the participating medium, if present
		MediumInteraction mi;
		// 采样下一个散射位置 或者 直接穿过体空间外部
		if (ray.medium) beta *= ray.medium->Sample(ray, sampler, &mi);
		if (beta.IsBlack()) break;

		// Handle an interaction with a medium or a surface
		if (mi.IsValid()) {
			// Terminate path if ray escaped or _maxDepth_ was reached
			if (bounces >= maxDepth) break;

			++volumeInteractions;
			// Handle scattering at point in medium for volumetric path tracer
			const Distribution1D *lightDistrib =
				lightDistribution->Lookup(mi.p);
			L += beta * UniformSampleOneLight(mi, scene, sampler, true, lightDistrib);

			Vector3f wo = -ray.d, wi;
			// 如果此时在体空间内部，则采样散射方向
			mi.phase->Sample_p(wo, &wi, sampler.Get2D());
			ray = mi.SpawnRay(wi);
			specularBounce = false;
		}
		else {
			++surfaceInteractions;
			// Handle scattering at point on surface for volumetric path tracer

			// Possibly add emitted light at intersection
			if (bounces == 0 || specularBounce) {
				// Add emitted light at path vertex or from the environment
				if (foundIntersection)
					L += beta * isect.Le(-ray.d);
				else
					for (const auto &light : scene.infiniteLights)
						L += beta * light->Le(ray);
			}

			// Terminate path if ray escaped or _maxDepth_ was reached
			if (!foundIntersection || bounces >= maxDepth) break;

			// Compute scattering functions and skip over medium boundaries
			isect.ComputeScatteringFunctions(ray, true);
			if (!isect.bsdf) {
				ray = isect.SpawnRay(ray.d);
				bounces--;
				continue;
			}

			// Sample illumination from lights to find attenuated path contribution
			const Distribution1D *lightDistrib =
				lightDistribution->Lookup(isect.p);
			L += beta * UniformSampleOneLight(isect, scene, sampler, true, lightDistrib);

			// Sample BSDF to get new path direction
			Vector3f wo = -ray.d, wi;
			float pdf;
			BxDFType flags;
			Spectrum f = isect.bsdf->Sample_f(wo, &wi, sampler.Get2D(), &pdf,
				BSDF_ALL, &flags);
			if (f.IsBlack() || pdf == 0.f) break;
			beta *= f * AbsDot(wi, isect.shading.n) / pdf;
			//DCHECK(std::isinf(beta.y()) == false);
			specularBounce = (flags & BSDF_SPECULAR) != 0;
			if ((flags & BSDF_SPECULAR) && (flags & BSDF_TRANSMISSION)) {
				float eta = isect.bsdf->eta;
				// Update the term that tracks radiance scaling for refraction
				// depending on whether the ray is entering or leaving the medium.
				etaScale *=
					(Dot(wo, isect.n) > 0) ? (eta * eta) : 1 / (eta * eta);
			}
			ray = isect.SpawnRay(wi);
		}

		// Possibly terminate the path with Russian roulette
		// Factor out radiance scaling due to refraction in rrBeta.
		Spectrum rrBeta = beta * etaScale;
		if (rrBeta.MaxComponentValue() < rrThreshold && bounces > 3) {
			float q = std::max((float).05, 1 - rrBeta.MaxComponentValue());
			if (sampler.Get1D() < q) break;
			beta /= 1 - q;
			//DCHECK(std::isinf(beta.y()) == false);
		}
	}


	return L;

}





}







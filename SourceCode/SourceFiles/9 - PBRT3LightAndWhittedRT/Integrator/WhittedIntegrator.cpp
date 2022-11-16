#include "Integrator\WhittedIntegrator.h"
#include "Core\Spectrum.h"
#include "Core\interaction.h"
#include "Core\Scene.h"
#include "Light\Light.h"
#include "Sampler\Sampler.h"
#include "Material\Reflection.h"


namespace Feimos {

Spectrum WhittedIntegrator::Li(const Ray &ray, const Scene &scene,
                               Sampler &sampler, int depth) const {
    Spectrum L(0.);
    // Find closest ray intersection or return background radiance
    SurfaceInteraction isect;
    if (!scene.Intersect(ray, &isect)) {
        for (const auto &light : scene.lights) L += light->Le(ray);
		if (depth <= 2) { 
			Spectrum LL;
			LL[2] = 0.8;
			return LL;
		}
        return L;
    }
	
    // Compute emitted and reflected light at ray intersection point

    // Initialize common variables for Whitted integrator
    const Normal3f &n = isect.shading.n;
    Vector3f wo = isect.wo;

    // Compute scattering functions for surface interaction
    isect.ComputeScatteringFunctions(ray);

	if (!isect.bsdf) return Li(isect.SpawnRay(ray.d), scene, sampler, depth);

    // Compute emitted light if ray hit an area light source
    //L += isect.Le(wo);

    // Add contribution of each light source
    for (const auto &light : scene.lights) {
        Vector3f wi;
        float pdf;
        VisibilityTester visibility;
        Spectrum Li = light->Sample_Li(isect, sampler.Get2D(), &wi, &pdf, &visibility);
        if (Li.IsBlack() || pdf == 0) continue;
        Spectrum f = isect.bsdf->f(wo, wi);
        if (!f.IsBlack() && visibility.Unoccluded(scene))
            L += f * Li * AbsDot(wi, n) / pdf;
    }
    if (depth + 1 < maxDepth) {
        // Trace rays for specular reflection and refraction
		L += SpecularReflect(ray, isect, scene, sampler, depth);
        //L += SpecularTransmit(ray, isect, scene, sampler, arena, depth);
    }
    return L;
}



}




















#pragma once
#ifndef __DirectLightingIntegrator_h__
#define __DirectLightingIntegrator_h__

#include "Core\FeimosRender.h"
#include "Integrator\Integrator.h"

namespace Feimos {

// LightStrategy Declarations
enum class LightStrategy { UniformSampleAll, UniformSampleOne };

// DirectLightingIntegrator Declarations
class DirectLightingIntegrator : public SamplerIntegrator {
  public:
    // DirectLightingIntegrator Public Methods
    DirectLightingIntegrator(LightStrategy strategy, int maxDepth,
                             std::shared_ptr<const Camera> camera,
                             std::shared_ptr<Sampler> sampler,
                             const Bounds2i &pixelBounds,
							FrameBuffer* framebuffer)
        : SamplerIntegrator(camera, sampler, pixelBounds, framebuffer),
          strategy(strategy),
          maxDepth(maxDepth) {}
    Spectrum Li(const Ray &ray, const Scene &scene,
                Sampler &sampler, int depth) const;
    void Preprocess(const Scene &scene, Sampler &sampler);

  private:
    // DirectLightingIntegrator Private Data
    const LightStrategy strategy;
    const int maxDepth;
    std::vector<int> nLightSamples;
};




}












#endif




#pragma once
#ifndef __PathIntegrator_h__
#define __PathIntegrator_h__

#include "Integrator/Integrator.h"
#include "Core/FeimosRender.h"

namespace Feimos
{

  // PathIntegrator Declarations
  class PathIntegrator : public SamplerIntegrator
  {
  public:
    // PathIntegrator Public Methods
    PathIntegrator(int maxDepth, std::shared_ptr<const Camera> camera,
                   std::shared_ptr<Sampler> sampler,
                   const Bounds2i &pixelBounds, float rrThreshold = 1,
                   const std::string &lightSampleStrategy = "spatial",
                   FrameBuffer *framebuffer = nullptr);

    void Preprocess(const Scene &scene, Sampler &sampler);
    Spectrum Li(const Ray &ray, const Scene &scene,
                Sampler &sampler, int depth) const;

  private:
    // PathIntegrator Private Data
    const int maxDepth;
    const float rrThreshold;
    const std::string lightSampleStrategy;
    std::unique_ptr<LightDistribution> lightDistribution;
  };

}

#endif

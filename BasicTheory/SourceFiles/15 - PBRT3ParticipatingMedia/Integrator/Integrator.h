#pragma once
#ifndef __Integrator_h__
#define __Integrator_h__
#include "Core/FeimosRender.h"
#include "Core/Geometry.h"
#include "Core/FrameBuffer.h"

namespace Feimos
{

	// Integrator Declarations
	class Integrator
	{
	public:
		// Integrator Interface
		virtual ~Integrator() {}
		virtual void Render(const Scene &scene, double &timeConsume) = 0;
		float IntegratorRenderTime; //渲染一次用的时间
	};

	Spectrum UniformSampleAllLights(const Interaction &it, const Scene &scene,
									Sampler &sampler,
									const std::vector<int> &nLightSamples,
									bool handleMedia = false);
	Spectrum UniformSampleOneLight(const Interaction &it, const Scene &scene,
								   Sampler &sampler,
								   bool handleMedia = false,
								   const Distribution1D *lightDistrib = nullptr);
	Spectrum EstimateDirect(const Interaction &it, const Point2f &uShading,
							const Light &light, const Point2f &uLight,
							const Scene &scene, Sampler &sampler,
							bool handleMedia = false,
							bool specular = false);

	// SamplerIntegrator Declarations
	class SamplerIntegrator : public Integrator
	{
	public:
		// SamplerIntegrator Public Methods
		SamplerIntegrator(std::shared_ptr<const Camera> camera,
						  std::shared_ptr<Sampler> sampler,
						  const Bounds2i &pixelBounds, FrameBuffer *m_FrameBuffer)
			: camera(camera), sampler(sampler), pixelBounds(pixelBounds), m_FrameBuffer(m_FrameBuffer) {}
		virtual void Preprocess(const Scene &scene, Sampler &sampler) {}
		void Render(const Scene &scene, double &timeConsume);

		virtual Spectrum Li(const RayDifferential &ray, const Scene &scene, Sampler &sampler, int depth = 0) const;
		Spectrum SpecularReflect(const RayDifferential &ray,
								 const SurfaceInteraction &isect,
								 const Scene &scene, Sampler &sampler,
								 int depth) const;
		Spectrum SpecularTransmit(const RayDifferential &ray,
								  const SurfaceInteraction &isect,
								  const Scene &scene, Sampler &sampler, int depth) const;

	protected:
		// SamplerIntegrator Protected Data
		std::shared_ptr<const Camera> camera;

	private:
		// SamplerIntegrator Private Data
		std::shared_ptr<Sampler> sampler;
		const Bounds2i pixelBounds;
		FrameBuffer *m_FrameBuffer;
	};

}

#endif

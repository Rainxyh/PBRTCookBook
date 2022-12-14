#pragma once
#ifndef __SCENE_H__
#define __SCENE_H__

#include "Core/FeimosRender.h"
#include "Core/Geometry.h"
#include "Core/Primitive.h"
#include <vector>

namespace Feimos
{

	class Scene
	{
	public:
		// Scene Public Methods
		Scene(std::shared_ptr<Primitive> aggregate,
			  const std::vector<std::shared_ptr<Light>> &lights);
		const Bounds3f &WorldBound() const { return worldBound; }
		bool Intersect(const Ray &ray, SurfaceInteraction *isect) const;
		bool IntersectP(const Ray &ray) const;
		bool IntersectTr(Ray ray, Sampler &sampler, SurfaceInteraction *isect,
						 Spectrum *transmittance) const;
		// Scene Public Data
		std::vector<std::shared_ptr<Light>> lights;
		std::vector<std::shared_ptr<Light>> infiniteLights;

	private:
		// Scene Private Data
		std::shared_ptr<Primitive> aggregate;
		Bounds3f worldBound;
	};

}

#endif

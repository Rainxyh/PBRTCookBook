#pragma once
#ifndef __SCENE_H__
#define __SCENE_H__

#include "Core\FeimosRender.h"
#include "Core\Geometry.hpp"
#include "Core\primitive.h"
#include <vector>

namespace Feimos{

class Scene {
public:
	// Scene Public Methods
	Scene(std::shared_ptr<Primitive> aggregate)
		: aggregate(aggregate) {
		// Scene Constructor Implementation
		worldBound = aggregate->WorldBound();
	}
	const Bounds3f &WorldBound() const { return worldBound; }
	bool Intersect(const Ray &ray, SurfaceInteraction *isect) const;
	bool IntersectP(const Ray &ray) const;

private:
	// Scene Private Data
	std::shared_ptr<Primitive> aggregate;
	Bounds3f worldBound;
};



}


#endif



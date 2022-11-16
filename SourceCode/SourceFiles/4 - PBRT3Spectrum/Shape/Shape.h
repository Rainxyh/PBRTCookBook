#pragma once
#ifndef __Shape_h__
#define __Shape_h__

#include "Core\FeimosRender.h"
#include "Core\Geometry.hpp"
#include "Core\Transform.h"

namespace Feimos {

class Shape {
public:
	// Shape Interface
	Shape(const Transform *ObjectToWorld, const Transform *WorldToObject,
		bool reverseOrientation);
	virtual ~Shape();
	virtual Bounds3f ObjectBound() const = 0;
	virtual Bounds3f WorldBound() const;
	virtual bool Intersect(const Ray &ray, float *tHit,
		SurfaceInteraction *isect,
		bool testAlphaTexture = true) const = 0;
	virtual bool IntersectP(const Ray &ray,
		bool testAlphaTexture = true) const {
		return Intersect(ray, nullptr, nullptr, testAlphaTexture);
	}
	virtual float Area() const = 0;

	const Transform *ObjectToWorld, *WorldToObject;
	const bool reverseOrientation;
};




}




#endif






#pragma once
#ifndef __Interaction_h__
#define __Interaction_h__

#include "Core/Geometry.hpp"
#include "Shape/Shape.h"

namespace Feimos
{

	struct Interaction
	{
		// Interaction Public Methods
		Interaction() : time(0) {}
		Interaction(const Point3f &p, const Normal3f &n, const Vector3f &pError,
					const Vector3f &wo, float time)
			: p(p),
			  time(time),
			  wo(Normalize(wo)),
			  n(n) {}
		// Interaction Public Data
		Point3f p;
		float time;
		Vector3f wo;
		Normal3f n;
	};

	class SurfaceInteraction : public Interaction
	{
	public:
		// SurfaceInteraction Public Methods
		SurfaceInteraction() {}
		void ComputeScatteringFunctions();
		const Shape *shape = nullptr;
		const Primitive *primitive = nullptr;
	};

}

#endif

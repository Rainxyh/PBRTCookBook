#pragma once
#ifndef __CONSTANT_MEDIUM_H__
#define __CONSTANT_MEDIUM_H__

#include "Core\Hitable.h"
#include "Core\TimeClockRandom.h"
#include "Core\texture.h"
#include "Core\Material.h"
#include <float.h>

class constant_medium :public hitable {
public:
	constant_medium(hitable *b, float d, texture *a) :boundary(b), density(d) {
		phase_function = new isotropic(a);
	}
	virtual bool hit(const Ray&r, float t_min, float t_max, hit_record&rec)const;
	virtual bool bounding_box(float t0, float t1, aabb&box)const {
		return boundary->bounding_box(t0, t1, box);
	}
	hitable *boundary;
	float density;
	material *phase_function;
};









#endif




















#pragma once
#ifndef __AABB_H__
#define __AABB_H__
#include "Ray.h"
#include <stdio.h>
inline float ffmin(float a, float b) { return a < b ? a : b; }
inline float ffmax(float a, float b) { return a > b ? a : b; }

class aabb
{
public:
	aabb() {}
	aabb(const vec3 &a, const vec3 &b)
	{
		_min = a;
		_max = b;
	}
	vec3 rmin() const { return _min; }
	vec3 rmax() const { return _max; }
	inline bool hit(const ray &r, float tmin, float tmax) const;
	vec3 _min;
	vec3 _max;
};
void swap(float &a, float &b)
{
	float c = a;
	a = b;
	b = c;
}
inline bool aabb::hit(const ray &r, float tmin, float tmax) const
{
	for (int a = 0; a < 3; a++)
	{
		float invD = 1.0f / r.direction()[a];
		float t0 = (rmin()[a] - r.origin()[a]) * invD;
		float t1 = (rmax()[a] - r.origin()[a]) * invD;
		if (invD < 0.0f)
			swap(t0, t1);
		tmin = t0 > tmin ? t0 : tmin;
		tmax = t1 < tmax ? t1 : tmax;
		if (tmax <= tmin)
			return false;
	}
	return true;
}

#endif

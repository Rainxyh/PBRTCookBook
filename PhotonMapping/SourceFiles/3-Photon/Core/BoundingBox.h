#pragma once
#ifndef BoundingBox
#define BoundingBox
#include "Core/Vector.h"
#include "Core/Ray.h"

inline float ffmin(float a, float b) { return a < b ? a : b; }
inline float ffmax(float a, float b) { return a > b ? a : b; }
inline float ffmin(float a, float b, float c)
{
	// 之前这里不小心写成int了，结果一直报错，调了整整一天。
	float s1 = a > b ? b : a;
	return s1 > c ? c : s1;
}
inline float ffmax(float a, float b, float c)
{
	float s1 = a > b ? a : b;
	return s1 > c ? s1 : c;
}

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
	bool hit(const Ray &r, float tmin, float tmax) const
	{
		for (int a = 0; a < 3; a++)
		{
			float t0 = ffmin((_min[a] - r.origin()[a]) / r.direction()[a],
							 (_max[a] - r.origin()[a]) / r.direction()[a]);
			float t1 = ffmax((_min[a] - r.origin()[a]) / r.direction()[a],
							 (_max[a] - r.origin()[a]) / r.direction()[a]);
			tmin = ffmax(t0, tmin);
			tmax = ffmin(t1, tmax);
			if (tmax <= tmin)
				return false;
		}
		return true;
	}

	vec3 _min;
	vec3 _max;
};

aabb surrounding_box(aabb box0, aabb box1);

#endif

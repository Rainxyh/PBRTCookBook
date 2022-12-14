#pragma once
#ifndef RAY_H
#define RAY_H

#include "Core/Vector.h"

class Ray
{
public:
	Ray() {}
	Ray(const vec3 &a, const vec3 &b, float ti = 0.0)
	{
		data[0] = a;
		data[1] = b;
		data[2] = vec3(1.0f / b.x(), 1.0f / b.y(), 1.0f / b.z());

		posneg[0] = (data[1].x() > 0 ? 0 : 1);
		posneg[1] = posneg[0] ^ 1;

		posneg[2] = (data[1].y() > 0 ? 0 : 1);
		posneg[3] = posneg[2] ^ 1;

		posneg[4] = (data[1].z() > 0 ? 0 : 1);
		posneg[5] = posneg[4] ^ 1;

		_time = ti;
	}
	Ray(const Ray &r) { *this = r; }
	vec3 origin() const { return data[0]; }
	vec3 direction() const { return data[1]; }
	vec3 invDirection() const { return data[2]; }
	float time() const { return _time; }
	void setOrigin(const vec3 &v) { data[0] = v; }
	void setDirection(const vec3 &v)
	{
		data[1] = v;
		data[2] = vec3(1.0f / v.x(), 1.0f / v.y(), 1.0f / v.z());

		posneg[0] = (data[1].x() > 0 ? 0 : 1);
		posneg[1] = posneg[0] ^ 1;

		posneg[2] = (data[1].y() > 0 ? 0 : 1);
		posneg[3] = posneg[2] ^ 1;

		posneg[4] = (data[1].z() > 0 ? 0 : 1);
		posneg[5] = posneg[4] ^ 1;
	}
	vec3 pointAtParameter(float t) const { return data[0] + t * data[1]; }
	// data[0]原点 data[1]方向 data[2] 1/方向
	vec3 data[3];
	// 方向是正的还是负的：posneg[0] x方向是不是负的 posneg[1] x方向是不是正的
	int posneg[6];
	float _time;
};

#endif

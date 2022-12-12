#pragma once
#ifndef __ONB_H__
#define __ONB_H__
#include "Core\Vector.h"


class onb {
public:
	onb() {}
	inline vec3 operator[](int i)const { return axis[i]; }
	vec3 u()const { return axis[0]; }
	vec3 v()const { return axis[1]; }
	vec3 w()const { return axis[2]; }
	vec3 local(float a, float b, float c)const { return a*u() + b*v() + c*w(); }
	vec3 local(const vec3&a)const { return a.x()*u() + a.y()*v() + a.z()*w(); }
	void build_from_w(const vec3&);
	vec3 axis[3];
};

inline void onb::build_from_w(const vec3&n) {
	axis[2] = unitVector(n);
	vec3 a;
	if (fabs(w().x()) > 0.9)
		a = vec3(0, 1, 0);
	else
		a = vec3(1, 0, 0);
	axis[1] = unitVector(cross(w(), a));
	axis[0] = cross(w(), v());
}
#endif
#pragma once
#ifndef __Triangle_h__
#define __Triangle_h__

#include "Hitable.h"
#include "Core\Material.h"
#include "Core\Vector.h"

#define EPSILON 0.000001
class triangle :public hitable {
public:
	triangle() {}
	triangle(vec3 a, vec3 b, vec3 c, material* thisMaterial) :V0(a), V1(b), V2(c), matral(thisMaterial) {
		NormalV0 = unitVector(cross(V1 - V0, V2 - V0));
		NormalV1 = unitVector(cross(V1 - V0, V2 - V0));
		NormalV2 = unitVector(cross(V1 - V0, V2 - V0));
	};
	triangle(vec3 a, vec3 b, vec3 c, vec3 Norm ,material* thisMaterial) :
		V0(a), V1(b), V2(c), NormalV0(Norm), NormalV1(Norm), NormalV2(Norm), matral(thisMaterial) {};
	triangle(vec3 a, vec3 b, vec3 c, vec3 Norm0, vec3 Norm1, vec3 Norm2, material* thisMaterial) :
		V0(a), V1(b), V2(c), NormalV0(Norm0), NormalV1(Norm1), NormalV2(Norm2), matral(thisMaterial) {};
	virtual bool hit(const Ray&r, float t_min, float t_max, hit_record&rec)const;
	virtual bool bounding_box(float t0, float t1, aabb&box)const;
	vec3 V0,V1,V2;
	vec3 NormalV0, NormalV1, NormalV2;
	material *matral;
};

















#endif





















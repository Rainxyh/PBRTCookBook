#pragma once
#pragma once

#ifndef __HITABLE__H__
#define __HITABLE__H__

#include "Ray.h"
#include "BoundingBox.h"
class pdf;
class material;
struct hit_record
{
	float t;
	vec3 p;
	vec3 normal;
	material *mat_ptr;
	float texU, texV;
};
struct scatter_record
{
	Ray specular_ray;
	bool is_specular;
	vec3 attenuation;
	pdf *pdf_ptr;
};
class hitable
{
public:
	virtual bool hit(const Ray &r, float t_min, float t_max, hit_record &rec) const = 0;
	virtual bool bounding_box(float t0, float t1, aabb &box) const = 0;
	virtual float pdf_value(const vec3 &o, const vec3 &v) const { return 0.0; }
	virtual vec3 randomSample(const vec3 &o) const { return vec3(1, 0, 0); }
};

class flip_normals : public hitable
{
public:
	flip_normals(hitable *p) : ptr(p) {}
	virtual bool hit(const Ray &r, float t_min, float t_max, hit_record &rec) const
	{
		if (ptr->hit(r, t_min, t_max, rec))
		{
			rec.normal = -rec.normal;
			return true;
		}
		else
			return false;
	}
	virtual bool bounding_box(float t0, float t1, aabb &box) const
	{
		return ptr->bounding_box(t0, t1, box);
	}
	hitable *ptr;
};

#endif

#pragma once
#ifndef __HITABLE__H__
#define __HITABLE__H__

#include "Ray.h"
#include "AABB.h"
#include "texture.h"
#include "ONB.h"

vec3 random_in_unit_sphere();
class material;
struct hit_record
{
	float t;
	vec3 p;
	vec3 normal;
	material *mat_ptr;
	float u, v;
};
class pdf;
struct scatter_record
{
	ray specular_ray;
	bool is_specular;
	vec3 albedo;
	pdf *pdf_ptr;
};
class hitable
{
public:
	virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const = 0;
	virtual bool bounding_box(float t0, float t1, aabb &box) const = 0;
	virtual float pdf_value(const vec3 &o, const vec3 &v) const { return 0.0; }
	virtual vec3 random(const vec3 &o) const { return vec3(1, 0, 0); }
};

class material
{
public:
	virtual bool scatter(const ray &r_in, const hit_record &rec, scatter_record &srec) const
	{
		return false;
	}
	virtual float scattering_pdf(const ray &r_in, const hit_record &rec, const ray &scattered) const
	{
		return false;
	}
	virtual vec3 emitted(const ray &r_in, const hit_record &rec, float u, float v, const vec3 &p) const
	{
		return vec3(0, 0, 0);
	}
};

#endif

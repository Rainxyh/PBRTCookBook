#pragma once
#ifndef __MATERIAL_H__
#define __MATERIAL_H__
#include "hitable.h"
#include "PDF.h"

inline vec3 random_in_unit_sphere()
{
	vec3 p;
	do
	{
		p = 2.0 * vec3(getClockRandom(), getClockRandom(), getClockRandom()) - vec3(1, 1, 1);
	} while (dot(p, p) >= 1.0);
	return p;
}

class lambertian : public material
{
public:
	lambertian(texture *a) : albedo(a) {}
	virtual float scattering_pdf(const ray &r_in, const hit_record &rec, const ray &scattered) const
	{
		float cosine = dot(rec.normal, unitVector(scattered.direction()));
		if (cosine < 0)
			cosine = 0;
		return cosine / M_PI;
	}
	virtual bool scatter(const ray &r_in, const hit_record &hrec, scatter_record &srec) const
	{
		srec.is_specular = false;
		srec.albedo = albedo->value(hrec.u, hrec.v, hrec.p);
		srec.pdf_ptr = new cosine_pdf(hrec.normal);
		return true;
	}
	texture *albedo;
};
class metal : public material
{
public:
	metal(const vec3 &a, float f) : albedo(a)
	{
		if (f < 1)
			fuzz = f;
		else
			fuzz = 1;
	}
	virtual bool scatter(const ray &r_in, const hit_record &hrec, scatter_record &srec) const
	{
		vec3 reflected = reflect(unitVector(r_in.direction()), hrec.normal);
		srec.specular_ray = ray(hrec.p, reflected + fuzz * random_in_unit_sphere());
		srec.albedo = albedo;
		srec.is_specular = true;
		srec.pdf_ptr = 0;
		return true;
	}
	vec3 albedo;
	float fuzz;
};
float schlick(float cosine, float ref_idx)
{
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}
class dielectric : public material
{
public:
	dielectric(float ri) : ref_idx(ri) {}
	virtual bool scatter(const ray &r_in, const hit_record &rec, scatter_record &srec) const
	{
		vec3 outward_normal;
		vec3 reflected = reflect((r_in.direction()), rec.normal);
		float ni_over_nt;
		srec.albedo = vec3(1.0, 1.0, 1.0);
		vec3 refracted;
		float reflect_prob;
		float cosine;
		srec.is_specular = true;
		//首先判断是射进球体内去的还是射出球体来的
		if (dot((r_in.direction()), rec.normal) > 0)
		{
			outward_normal = -rec.normal;
			ni_over_nt = ref_idx;
			cosine = ref_idx * dot((r_in.direction()), rec.normal) / (r_in.direction()).length();
		}
		else
		{
			outward_normal = rec.normal;
			ni_over_nt = 1.0 / ref_idx;
			cosine = -dot((r_in.direction()), rec.normal) / (r_in.direction()).length();
		}
		//判断是折射还是全反射
		if (refract((r_in.direction()), outward_normal, ni_over_nt, refracted))
		{
			//就算是折射，也得生成一下概率
			reflect_prob = schlick(cosine, ref_idx);
		}
		else
		{
			srec.specular_ray = ray(rec.p, reflected);
			reflect_prob = 1.0;
		}

		if (getClockRandom() < reflect_prob)
		{
			srec.specular_ray = ray(rec.p, reflected);
		}
		else
		{
			srec.specular_ray = ray(rec.p, refracted);
		}
		return true;
	}
	float ref_idx;
};

class diffuse_light : public material
{
public:
	diffuse_light(texture *a) : emitter(a) {}
	virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered, float &pdf) const
	{
		return false;
	}
	virtual vec3 emitted(const ray &r_in, const hit_record &rec, float u, float v, const vec3 &p) const
	{
		if (dot(rec.normal, r_in.direction()) < 0.0)
			return emitter->value(u, v, p);
		else
			return vec3(0, 0, 0);
	}
	texture *emitter;
};

#endif

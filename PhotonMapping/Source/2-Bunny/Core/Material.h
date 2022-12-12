#pragma once
#ifndef _MATERIAL_H__
#define _MATERIAL_H__

#include "Core\Hitable.h"
#include "Core\texture.h"
#include "Core\ONB.h"
#include "Core\PDF.h"
#include "Core\TimeClockRandom.h"

class material {
public:
	virtual float scattering_pdf(const Ray &r_in, const hit_record& rec, const Ray& scattered)const {
		return 0.0f;
	}
	virtual bool scatter(const Ray &r_in, const hit_record& hrec, scatter_record& srec)const {
		return false;
	}
	virtual vec3 emitted(const Ray&r_in, const hit_record&rec, float u, float v, const vec3&p)const {
		return vec3(0, 0, 0);
	}
};

class lambertian :public material {
public:
	lambertian(texture*a) :texL(a) {}
	virtual bool scatter(const Ray &r_in, const hit_record& hrec, scatter_record& srec)const {
		srec.is_specular = false;
		srec.albedo = texL->value(hrec.texU, hrec.texV, hrec.p);
		srec.pdf_ptr = new cosine_pdf(hrec.normal);
		return true;
	}
	virtual float scattering_pdf(const Ray &r_in, const hit_record& rec, const Ray& scattered)const {
		float cosine = dot(rec.normal, unitVector(scattered.direction()));
		if (cosine < 0)cosine = 0;
		return cosine / Pi;
	}
	texture* texL;
};

class metal :public material {
public:
	metal(const vec3&a, float f) :albedo(a) { if (f < 1)fuzz = f; else fuzz = 1; }
	virtual bool scatter(const Ray& r_in, const hit_record& hrec, scatter_record& srec)const {
		vec3 reflected = reflect(unitVector(r_in.direction()), hrec.normal);
		srec.specular_ray = Ray(hrec.p, reflected + random_in_unit_sphere()*fuzz);
		srec.albedo = albedo;
		srec.is_specular = true;
		srec.pdf_ptr = 0;
		return true;
	}
	vec3 albedo;
	float fuzz;
};

class roughMetal :public material {
public:
	roughMetal(texture*a, float theta = 10.0f) :texL(a) {}
	virtual bool scatter(const Ray &r_in, const hit_record& hrec, scatter_record& srec)const {
		srec.is_specular = false;
		srec.albedo = texL->value(hrec.texU, hrec.texV, hrec.p);
		srec.pdf_ptr = new cosine_pdf(hrec.normal);
		return true;
	}
	virtual float scattering_pdf(const Ray &r_in, const hit_record& rec, const Ray& scattered)const {
		float cosine = dot(rec.normal, unitVector(scattered.direction()));
		if (cosine < 0)cosine = 0;
		return cosine / Pi;
	}
	texture* texL;
};

class glossy :public material {
public:
	glossy(const vec3&a, float f) :albedo(a) { if (f < 1)fuzz = f; else fuzz = 1; }
	virtual bool scatter(const Ray& r_in, const hit_record& hrec, scatter_record& srec)const {
		vec3 reflected = reflect(unitVector(r_in.direction()), hrec.normal);
		srec.specular_ray = Ray(hrec.p, reflected + random_in_unit_sphere()*fuzz);
		srec.albedo = albedo;
		srec.is_specular = true;
		srec.pdf_ptr = 0;
		return true;
	}
	vec3 albedo;
	float fuzz;
};

inline float schlick(float cosine, float ref_idx) {
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0*r0;
	return r0 + (1 - r0)*pow((1 - cosine), 5);
}
class dielectric :public material {
public:
	dielectric(float ri, vec3 alb = vec3(1.0, 1.0, 1.0), float atn = 0.2) : ref_idx(ri), albedo(alb), attenu(atn) { }
	virtual bool scatter(const Ray&r_in, const hit_record&rec, scatter_record& srec)const {
		srec.is_specular = true;
		srec.pdf_ptr = 0;
		vec3 outward_normal;
		float ni_over_nt;
		bool isOut, isRfra;//是不是射入和折射
		vec3 refracted;
		float reflect_prob;
		float cosine;
		//float attenLength = (rec.p - rec.p_last).length();
		//attenLength = powf((1.0f - attenu),attenLength+1);
		//首先判断是射进球体内去的还是射出球体来的
		if (dot(unitVector(r_in.direction()), rec.normal) > 0) {
			//射出
			isOut = true;
			outward_normal = -rec.normal;
			ni_over_nt = ref_idx;
			cosine = ref_idx*dot(r_in.direction(), rec.normal) / r_in.direction().length();
		}
		else {
			//射入
			isOut = true;
			outward_normal = rec.normal;
			ni_over_nt = 1.0 / ref_idx;
			cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
		}
		//判断是折射还是全反射
		if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
			//玻璃的镜面效应的概率
			reflect_prob = schlick(cosine, ref_idx);
		}
		else {
			reflect_prob = 1.0;
		}
		if (getClockRandom() < reflect_prob) {
			//全反射
			isRfra = false;
			vec3 reflected = reflect(r_in.direction(), rec.normal);
			srec.specular_ray = Ray(rec.p, reflected, r_in.time());
		}
		else {
			//折射
			isRfra = true;
			srec.specular_ray = Ray(rec.p, refracted, r_in.time());
		}
		//只有从介质中射出，才有介质吸收
		if (isOut && isRfra) {
			srec.albedo = albedo;
		}
		else {
			srec.albedo = vec3(1.0, 1.0, 1.0);
		}
		return true;
	}
	float ref_idx;
	float attenu;//保留功能，计算在介质中的衰减
	vec3 albedo;
};

class diffuse_light :public material {
public:
	diffuse_light(texture *a) :emission(a) {}
	virtual bool scatter(const Ray&r_in, const hit_record&rec, vec3&attenuation, Ray& scattered, float &pdf)const
	{
		return false;
	}
	virtual vec3 emitted(const Ray&r_in, const hit_record&rec, float u, float v, const vec3&p)const {
		if (dot(rec.normal, r_in.direction()) < 0.0)
			return emission->value(u, v, p);
		else
			return vec3(0, 0, 0);
	}
	texture *emission;
};


class isotropic :public material
{
public:
	isotropic(texture* tex) :albedo(tex) {  }
	virtual bool scatter(const Ray&r_in, const hit_record&rec, vec3&attenuation, Ray& scattered, float &pdf)const
	{
		scattered = Ray(rec.p, random_in_unit_sphere());
		attenuation = albedo->value(rec.texU, rec.texV, rec.p);
		return true;
	}
private:
	texture * albedo; //就用漫反射来命名
};






#endif









#pragma once
#ifndef __SPHEREH__
#define __SPHEREH__

#include "Hitable.h"
class sphere : public hitable
{
public:
	sphere() {}
	sphere(vec3 cen, float r, material *thisMaterial) : center(cen), radius(r), matral(thisMaterial){};
	virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
	virtual bool bounding_box(float t0, float t1, aabb &box) const;
	virtual float pdf_value(const vec3 &o, const vec3 &v) const;
	virtual vec3 random(const vec3 &o) const;
	vec3 center;
	float radius;
	material *matral;
};
float sphere::pdf_value(const vec3 &o, const vec3 &v) const
{
	hit_record rec;
	if (this->hit(ray(o, v), 0.001, FLT_MAX, rec))
	{
		float cos_theta_max = sqrt(1 - radius * radius / (center - o).squaredLength());
		float solid_angle = 2 * M_PI * (1 - cos_theta_max);
		return 1 / solid_angle;
	}
	else
	{
		return 0;
	}
}
inline vec3 random_to_sphere(float radius, float distance_squared)
{
	float r1 = getClockRandom();
	float r2 = getClockRandom();
	float z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);
	float phi = 2 * M_PI * r1;
	float x = cos(phi) * sqrt(1 - z * z);
	float y = sin(phi) * sqrt(1 - z * z);
	return vec3(x, y, z);
}
vec3 sphere::random(const vec3 &o) const
{
	vec3 direction = center - o;
	float distance_squared = direction.squaredLength();
	onb uvw;
	uvw.build_from_w(direction);
	return uvw.local(random_to_sphere(radius, distance_squared));
}
bool sphere::hit(const ray &r, float t_min, float t_max, hit_record &rec) const
{
	vec3 oc = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	float b = 2.0 * dot(oc, r.direction());
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - 4 * a * c;
	if (discriminant > 0)
	{
		float temp = (-b - sqrt(discriminant)) / (2.0 * a);
		if (temp < t_max && temp > t_min)
		{
			rec.t = temp;
			rec.p = r.pointAtParameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.mat_ptr = matral;
			get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);
			return true;
		}
		temp = (-b + sqrt(discriminant)) / (2.0 * a);
		if (temp < t_max && temp > t_min)
		{
			rec.t = temp;
			rec.p = r.pointAtParameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.mat_ptr = matral;
			return true;
		}
	}
	return false;
}

bool sphere::bounding_box(float t0, float t1, aabb &box) const
{
	box = aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
	return true;
}

class moving_sphere : public hitable
{
public:
	moving_sphere() {}
	moving_sphere(vec3 cen0, vec3 cen1, float t0, float t1, float r, material *m) : center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), mat_ptr(m){};
	virtual bool hit(const ray &r, float tmin, float tmax, hit_record &rec) const;
	virtual bool bounding_box(float t0, float t1, aabb &box) const;
	vec3 center(float time) const;
	vec3 center0, center1;
	float time0, time1;
	float radius;
	material *mat_ptr;
};

aabb surrounding_box(aabb box0, aabb box1)
{
	vec3 small(fmin(box0.rmin().x(), box1.rmin().x()),
			   fmin(box0.rmin().y(), box1.rmin().y()),
			   fmin(box0.rmin().z(), box1.rmin().z()));
	vec3 big(fmax(box0.rmax().x(), box1.rmax().x()),
			 fmax(box0.rmax().y(), box1.rmax().y()),
			 fmax(box0.rmax().z(), box1.rmax().z()));
	return aabb(small, big);
}

bool moving_sphere::bounding_box(float t0, float t1, aabb &box) const
{
	vec3 v_radius(radius, radius, radius);
	box = surrounding_box(aabb(center0 - v_radius, center0 + v_radius), aabb(center1 - v_radius, center1 + v_radius));
	return true;
}
vec3 moving_sphere::center(float time) const
{
	return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}

bool moving_sphere::hit(const ray &r, float t_min, float t_max, hit_record &rec) const
{
	vec3 oc = r.origin() - center(r.time());
	float a = dot(r.direction(), r.direction());
	float b = 2.0 * dot(oc, r.direction());
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - 4 * a * c;
	if (discriminant > 0)
	{
		float temp = (-b - sqrt(discriminant)) / (2.0 * a);
		if (temp < t_max && temp > t_min)
		{
			rec.t = temp;
			rec.p = r.pointAtParameter(rec.t);
			rec.normal = (rec.p - center(r.time())) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
		temp = (-b + sqrt(discriminant)) / (2.0 * a);
		if (temp < t_max && temp > t_min)
		{
			rec.t = temp;
			rec.p = r.pointAtParameter(rec.t);
			rec.normal = (rec.p - center(r.time())) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}
	return false;
}

#endif

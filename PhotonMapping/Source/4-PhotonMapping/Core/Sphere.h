#pragma once
#pragma once
#ifndef __SPHEREH__
#define __SPHEREH__

#include "Hitable.h"
#include "Core\Material.h"
inline void get_sphere_uv(const vec3&p, float &u, float &v) {
	float phi = atan2(p.z(), p.x());
	float theta = asin(p.y());
	u = 1 - (phi + Pi) / (2 * Pi);
	v = (theta + Pi / 2) / Pi;
}
class sphere :public hitable {
public:
	sphere() {}
	sphere(vec3 cen, float r, material* thisMaterial) :center(cen), radius(r), matral(thisMaterial) {};

	virtual bool hit(const Ray&r, float t_min, float t_max, hit_record&rec)const;
	virtual bool bounding_box(float t0, float t1, aabb& box)const;
	float sphere::pdf_value(const vec3& o, const vec3& v) const {
		hit_record rec;
		if (this->hit(Ray(o, v), 0.001, FLT_MAX, rec)) {
			float cos_theta_max = sqrt(1 - radius*radius / (center - o).squaredLength());
			float solid_angle = 2 * Pi*(1 - cos_theta_max);
			return 1 / solid_angle;
		}
		else {
			return 0;
		}
	}
	vec3 sphere::randomSample(const vec3& o) const {
		vec3 direction = center - o;
		float distance_squared = direction.squaredLength();
		onb uvw;
		uvw.build_from_w(direction);
		return uvw.local(random_to_sphere(radius, distance_squared));
	}
	vec3 center;
	float radius;
	material *matral;
};
inline bool sphere::hit(const Ray&r, float t_min, float t_max, hit_record&rec)const {
	vec3 oc = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	float b = 2.0*dot(oc, r.direction());
	float c = dot(oc, oc) - radius*radius;
	float discriminant = b*b - 4 * a*c;
	if (discriminant > 0) {
		float temp = (-b - sqrt(discriminant)) / (2.0*a);
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.pointAtParameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.mat_ptr = matral;
			get_sphere_uv((rec.p - center) / radius, rec.texU, rec.texV);
			return true;
		}
		temp = (-b + sqrt(discriminant)) / (2.0*a);
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.pointAtParameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.mat_ptr = matral;
			return true;
		}
	}
	return false;
}
inline bool sphere::bounding_box(float t0, float t1, aabb& box)const {
	box = aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
	return true;
}


class moving_sphere :public hitable {
public:
	moving_sphere() {}
	moving_sphere(vec3 cen0, vec3 cen1, float t0, float t1, float r, material*m) :
		center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), mat_ptr(m) {};
	virtual bool hit(const Ray&r, float tmin, float tmax, hit_record& rec)const;
	virtual bool moving_sphere::bounding_box(float t0, float t1, aabb&box)const;
	vec3 center(float time)const;
	vec3 center0, center1;
	float time0, time1;
	float radius;
	material* mat_ptr;
};
inline vec3 moving_sphere::center(float time)const {
	return center0 + ((time - time0) / (time1 - time0))*(center1 - center0);
}
inline bool moving_sphere::hit(const Ray&r, float t_min, float t_max, hit_record&rec)const {
	vec3 oc = r.origin() - center(r.time());
	float a = dot(r.direction(), r.direction());
	float b = 2.0*dot(oc, r.direction());
	float c = dot(oc, oc) - radius*radius;
	float discriminant = b*b - 4 * a*c;
	if (discriminant > 0) {
		float temp = (-b - sqrt(discriminant)) / (2.0*a);
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.pointAtParameter(rec.t);
			rec.normal = (rec.p - center(r.time())) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
		temp = (-b + sqrt(discriminant)) / (2.0*a);
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.pointAtParameter(rec.t);
			rec.normal = (rec.p - center(r.time())) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}
	return false;
}
inline bool moving_sphere::bounding_box(float t0, float t1, aabb&box)const {
	vec3 v_radius(radius, radius, radius);
	box = surrounding_box(aabb(center0 - v_radius, center0 + v_radius), aabb(center1 - v_radius, center1 + v_radius));
	return true;
}







#endif



















#pragma once
#ifndef __RECTANGLE_H__
#define __RECTANGLE_H__
#include "hitable.h"
#include "hitablelist.h"

class xy_rect :public hitable {
public:
	xy_rect() {}
	xy_rect(float _x0, float _x1, float _y0, float _y1, float _k, material*mat) :
		x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {};
	virtual bool hit(const ray&r, float t0, float t1, hit_record &rec)const;
	virtual bool bounding_box(float t0, float t1, aabb&box)const {
		box = aabb(vec3(x0, y0, k - 0.0001), vec3(x1, y1, k + 0.0001));
		return true;
	}
	material *mp;
	float x0, x1, y0, y1, k;
};
class xz_rect :public hitable {
public:
	xz_rect() {}
	xz_rect(float _x0, float _x1, float _z0, float _z1, float _k, material*mat) :
		x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {};
	virtual bool hit(const ray&r, float t0, float t1, hit_record &rec)const;
	virtual bool bounding_box(float t0, float t1, aabb&box)const {
		box = aabb(vec3(x0, k - 0.0001, z0), vec3(x1, k + 0.0001, z1));
		return true;
	}
	virtual float pdf_value(const vec3& o, const vec3& v) const
	{
		hit_record rec;
		if (this->hit(ray(o, v), 0.001f, FLT_MAX, rec))
		{
			float area = (x1 - x0)*(z1 - z0);
			float distance_squared = rec.t * rec.t * v.squaredLength();
			float cosine = fabs(dot(v, rec.normal) / v.length());
			return distance_squared / (cosine*area);
		}
		else
			return 0;
	}

	virtual vec3 random(const vec3& o) const
	{
		vec3 random_point = vec3(x0 + getClockRandom()*(x1 - x0), k, z0 + getClockRandom()*(z1 - z0));
		return random_point - o;
	}
	material *mp;
	float x0, x1, z0, z1, k;
};
class yz_rect :public hitable {
public:
	yz_rect() {}
	yz_rect(float _y0, float _y1, float _z0, float _z1, float _k, material*mat) :
		y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {};
	virtual bool hit(const ray&r, float t0, float t1, hit_record &rec)const;
	virtual bool bounding_box(float t0, float t1, aabb&box)const {
		box = aabb(vec3( k - 0.0001,y0, z0), vec3( k + 0.0001,y1, z1));
		return true;
	}
	material *mp;
	float y0, y1, z0, z1, k;
};

class flip_normals :public hitable {
public:
	flip_normals(hitable*p):ptr(p){}
	virtual bool hit(const ray&r,float t_min,float t_max,hit_record&rec)const{
		if (ptr->hit(r, t_min, t_max, rec)) {
			rec.normal = -rec.normal;
			return true;
		}
		else
			return false;
	}
	virtual bool bounding_box(float t0, float t1, aabb&box)const {
		return ptr->bounding_box(t0, t1, box);
	}
	hitable *ptr;
};

bool xy_rect::hit(const ray&r, float t0, float t1, hit_record&rec)const {
	float t = (k - r.origin().z()) / r.direction().z();
	if (t<t0 || t>t1)
		return false;
	float x = r.origin().x() + t*r.direction().x();
	float y = r.origin().y() + t*r.direction().y();
	if (x < x0 || x > x1 || y < y0 || y > y1)
		return false;
	rec.u = (x - x0) / (x1 - x0);
	rec.v = (y - y0) / (y1 - y0);
	rec.t = t;
	rec.mat_ptr = mp;
	rec.p = r.pointAtParameter(t);
	rec.normal = vec3(0, 0, 1);
	return true;
}
bool xz_rect::hit(const ray&r, float t0, float t1, hit_record&rec)const {
	float t = (k - r.origin().y()) / r.direction().y();
	if (t<t0 || t>t1)
		return false;
	float x = r.origin().x() + t*r.direction().x();
	float z = r.origin().z() + t*r.direction().z();
	if (x < x0 || x > x1 || z < z0 || z > z1)
		return false;
	rec.u = (x - x0) / (x1 - x0);
	rec.v = (z - z0) / (z1 - z0);
	rec.t = t;
	rec.mat_ptr = mp;
	rec.p = r.pointAtParameter(t);
	rec.normal = vec3(0, 1, 0);
	return true;
}
bool yz_rect::hit(const ray&r, float t0, float t1, hit_record&rec)const {
	float t = (k - r.origin().x()) / r.direction().x();
	if (t<t0 || t>t1)
		return false;
	float y = r.origin().y() + t*r.direction().y();
	float z = r.origin().z() + t*r.direction().z();
	if (y < y0 || y > y1 || z < z0 || z > z1)
		return false;
	rec.u = (y - y0) / (y1 - y0);
	rec.v = (z - z0) / (z1 - z0);
	rec.t = t;
	rec.mat_ptr = mp;
	rec.p = r.pointAtParameter(t);
	rec.normal = vec3(1, 0, 0);
	return true;
}

class box :public hitable {
public:
	box(){}
	box(const vec3&p0, const vec3&p1, material *ptr);
	virtual bool hit(const ray&r, float t0, float t1, hit_record&rec)const;
	virtual bool bounding_box(float t0, float t1, aabb&box)const {
		box = aabb(pmin, pmax);
		return true;
	}
	vec3 pmin, pmax;
	hitable *list_ptr;
};

box::box(const vec3&p0, const vec3&p1, material *ptr) {
	pmin = p0;
	pmax = p1;
	hitable **list = new hitable*[6];
	list[0] = new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr);
	list[1] = new flip_normals(new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));
	list[2] = new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr);
	list[3] = new flip_normals(new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));
	list[4] = new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr);
	list[5] = new flip_normals(new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
	list_ptr = new hitable_list(list, 6);
}

bool box::hit(const ray&r, float t0, float t1, hit_record&rec)const {
	return list_ptr->hit(r, t0, t1, rec);
}

#endif




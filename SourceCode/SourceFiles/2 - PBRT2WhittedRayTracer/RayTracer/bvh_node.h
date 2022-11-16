#pragma once
#ifndef __bvh_node_h__
#define __bvh_node_h__
#include "AABB.h"
#include "hitable.h"
#include <math.h>
#include <stdlib.h>

class bvh_node :public hitable {
public:
	bvh_node() {left = NULL;right = NULL; }
	bvh_node(hitable **l, int n, float time0, float time1);
	virtual bool hit(const ray&r, float t_min, float t_max, hit_record&rec)const;
	virtual bool bounding_box(float t0, float t1, aabb&box)const;
	hitable *left;
	hitable *right;
	aabb box;
};

inline bool bvh_node::bounding_box(float t0, float t1, aabb& b)const {
	b = box;
	return true;
}

bool bvh_node::hit(const ray&r, float t_min, float t_max, hit_record &rec)const {
	if (box.hit(r, t_min, t_max)) {
		
		hit_record left_rec, right_rec;
		bool hit_left = left->hit(r, t_min, t_max, left_rec);
		bool hit_right = right->hit(r, t_min, t_max, right_rec);
		if (hit_left && hit_right) {
			if (left_rec.t < right_rec.t)
				rec = left_rec;
			else
				rec = right_rec;
			return true;
		}
		else if (hit_left) {
			rec = left_rec;
			return true;
		}
		else if (hit_right) {
			rec = right_rec;
			return true;
		}
		else
			return false;
	}
	else return false;
}

int box_x_compare(const void *a, const void *b) {
	aabb box_left, box_right;
	hitable *ah = *(hitable**)a;
	hitable *bh = *(hitable**)b;
	//if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
	//	return 0;//输出错误信息
	if (box_left.rmin().x() - box_right.rmin().x() < 0.0)return -1;
	else return 1;
}
int box_y_compare(const void *a, const void *b) {
	aabb box_left, box_right;
	hitable *ah = *(hitable**)a;
	hitable *bh = *(hitable**)b;
	if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
		return 0;//输出错误信息
	if (box_left.rmin().y() - box_right.rmin().y() < 0.0)return -1;
	else return 1;
}
int box_z_compare(const void *a, const void *b) {
	aabb box_left, box_right;
	hitable *ah = *(hitable**)a;
	hitable *bh = *(hitable**)b;
	if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
		return 0;//输出错误信息
	if (box_left.rmin().z() - box_right.rmin().z() < 0.0)return -1;
	else return 1;
}

aabb surrounding_box(aabb box0, aabb box1) {
	vec3 small(fmin(box0.rmin().x(), box1.rmin().x()),
		fmin(box0.rmin().y(), box1.rmin().y()),
		fmin(box0.rmin().z(), box1.rmin().z()));
	vec3 big(fmax(box0.rmax().x(), box1.rmax().x()),
		fmax(box0.rmax().y(), box1.rmax().y()),
		fmax(box0.rmax().z(), box1.rmax().z()));
	return aabb(small, big);
}

bvh_node::bvh_node(hitable **l, int n, float time0, float time1) {
	left = NULL;
	right = NULL;
	int axis = int(3 * getClockRandom());
	if (axis == 0)
		qsort(l, n, sizeof(hitable*), box_x_compare);
	else if (axis == 1)
		qsort(l, n, sizeof(hitable *), box_y_compare);
	else
		qsort(l, n, sizeof(hitable *), box_z_compare);

	if (n == 1) {
		left = right = l[0];
	}
	else if (n == 2) {
		left = l[0];
		right = l[1];
	}
	else {
		left = new bvh_node(l, n / 2, time0, time1);
		right = new bvh_node(l + n / 2, n - n / 2, time0, time1);
	}
	aabb box_left, box_right;
	if (!left->bounding_box(time0, time1, box_left) || !right->bounding_box(time0, time1, box_right)) { }
	box = surrounding_box(box_left, box_right);
}




#endif




#pragma once
#ifndef __bvhTree_h__
#define __bvhTree_h__
#include "Core\Hitable.h"

class bvh_node :public hitable {
public:
	bvh_node() {}
	bvh_node(hitable **l, int n, float time0, float time1);
	virtual bool hit(const Ray&r, float t_min, float t_max, hit_record&rec)const;
	virtual bool bounding_box(float t0, float t1, aabb&box)const;
	hitable *left;
	hitable *right;
	aabb box;
};








#endif





















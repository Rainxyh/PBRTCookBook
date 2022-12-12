#pragma once
#ifndef __PERLIN_H__
#define __PERLIN_H__

#include"Core\Vector.h"
#include <stdio.h>
#include "time.h"
#include "Core\TimeClockRandom.h"

inline float trilinear_interp(float c[2][2][2], float u, float v, float w);
class perlin {
public:
	float noise(const vec3& p)const;
	static float *ranfloat;
	static vec3 *ranvec;
	static int *perm_x;
	static int *perm_y;
	static int *perm_z;
	void print();
	float turb(const vec3&p, int depth = 7)const;
};

static float* perlin_generate();
void permute(int*p, int n);
static int* perlin_generate_perm();
static vec3* perlin_generateVec();
inline float perlin_interp(vec3 c[2][2][2], float u, float v, float w);







#endif

























#ifndef __TimeClockRandom_h__
#define __TimeClockRandom_h__

#include <stdlib.h>
#include <time.h>
#include "FeimosRender.h"
#include "Vector.h"

inline void ClockRandomInit()
{
	srand((unsigned)time(NULL));
}
inline double getClockRandom()
{
	return rand() / (RAND_MAX + 1.0);
}

inline vec3 random_in_unit_sphere()
{
	vec3 p;
	do
	{
		p = 2.0 * vec3(getClockRandom(), getClockRandom(), getClockRandom()) - vec3(1, 1, 1);
	} while (dot(p, p) >= 1.0);
	return p;
}

inline vec3 random_in_unit_disk()
{
	vec3 p;
	do
	{
		p = 2.0 * vec3(rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0), 0.0) - vec3(1, 1, 0);
	} while (dot(p, p) >= 1.0);
	return p;
}

inline vec3 random_cosine_direction()
{
	float r1 = getClockRandom();
	float r2 = getClockRandom();
	float z = sqrt(1 - r2);
	float phi = 2 * Pi * r1;
	float x = cos(phi) * 2 * sqrt(r2);
	float y = sin(phi) * 2 * sqrt(r2);
	return vec3(x, y, z);
}

inline vec3 random_to_sphere(float radius, float distance_squared)
{
	float r1 = getClockRandom();
	float r2 = getClockRandom();
	float z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);
	float phi = 2 * Pi * r1;
	float x = cos(phi) * sqrt(1 - z * z);
	float y = sin(phi) * sqrt(1 - z * z);
	return vec3(x, y, z);
}

inline vec3 de_nan(const vec3 &c)
{
	vec3 temp = c;
	if (!(temp[0] == temp[0]))
		temp[0] = 0;
	if (!(temp[1] == temp[1]))
		temp[1] = 0;
	if (!(temp[2] == temp[2]))
		temp[2] = 0;
	return temp;
}

#endif

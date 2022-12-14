#pragma once
#ifndef VECTOR3_H
#define VECTOR3_H

#include "FeimosRender.h"

#include <math.h>
#include <stdlib.h>

#define FLT_MAX 1000000.0f

class vec3
{
public:
	vec3()
	{
		e[0] = 0;
		e[1] = 0;
		e[2] = 0;
	}
	vec3(float e0, float e1, float e2)
	{
		e[0] = e0;
		e[1] = e1;
		e[2] = e2;
	}
	inline vec3(const vec3 &v)
	{
		e[0] = v.e[0];
		e[1] = v.e[1];
		e[2] = v.e[2];
	}

	float x() const { return e[0]; }
	float y() const { return e[1]; }
	float z() const { return e[2]; }
	void setX(float a) { e[0] = a; }
	void setY(float a) { e[1] = a; }
	void setZ(float a) { e[2] = a; }

	const vec3 &operator+() const { return *this; }
	vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
	float &operator[](int i) { return e[i]; }
	float operator[](int i) const { return e[i]; }
	vec3 &operator+=(const vec3 &v2);
	vec3 &operator-=(const vec3 &v2);
	vec3 &operator*=(const float t);
	vec3 &operator/=(const float t);

	float length() const { return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); }
	float squaredLength() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }
	void makeUnitVector();

	float minComponent() const { return e[indexOfMinComponent()]; }
	float maxComponent() const { return e[indexOfMaxComponent()]; }
	float maxAbsComponent() const { return e[indexOfMaxAbsComponent()]; }
	float minAbsComponent() const { return e[indexOfMinAbsComponent()]; }
	int indexOfMinComponent() const
	{
		return (e[0] < e[1] && e[0] < e[2]) ? 0 : (e[1] < e[2] ? 1 : 2);
	}
	int indexOfMinAbsComponent() const
	{
		if (fabs(e[0]) < fabs(e[1]) && fabs(e[0]) < fabs(e[2]))
			return 0;
		else if (fabs(e[1]) < fabs(e[2]))
			return 1;
		else
			return 2;
	}
	int indexOfMaxComponent() const
	{
		return (e[0] > e[1] && e[0] > e[2]) ? 0 : (e[1] > e[2] ? 1 : 2);
	}
	int indexOfMaxAbsComponent() const
	{
		if (fabs(e[0]) > fabs(e[1]) && fabs(e[0]) > fabs(e[2]))
			return 0;
		else if (fabs(e[1]) > fabs(e[2]))
			return 1;
		else
			return 2;
	}
	float e[3];
};

inline vec3 unitVector(const vec3 &v)
{
	float k = 1.0f / sqrt(v.e[0] * v.e[0] + v.e[1] * v.e[1] + v.e[2] * v.e[2]);
	return vec3(v.e[0] * k, v.e[1] * k, v.e[2] * k);
}
inline void vec3::makeUnitVector()
{
	float k = 1.0f / sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
	e[0] *= k;
	e[1] *= k;
	e[2] *= k;
}
inline float dot(const vec3 &v1, const vec3 &v2)
{
	return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}
inline vec3 cross(const vec3 &v1, const vec3 &v2)
{
	return vec3((v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
				(v1.e[2] * v2.e[0] - v1.e[0] * v2.e[2]),
				(v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]));
}

inline bool operator==(const vec3 &t1, const vec3 &t2)
{
	return ((t1[0] == t2[0]) && (t1[1] == t2[1]) && (t1[2] == t2[2]));
}
inline bool operator!=(const vec3 &t1, const vec3 &t2)
{
	return ((t1[0] != t2[0]) || (t1[1] != t2[1]) || (t1[2] != t2[2]));
}
inline vec3 operator+(const vec3 &v1, const vec3 &v2)
{
	return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}
inline vec3 operator-(const vec3 &v1, const vec3 &v2)
{
	return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}
inline vec3 operator*(float t, const vec3 &v)
{
	return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}
inline vec3 operator*(const vec3 &v, float t)
{
	return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}
inline vec3 operator*(const vec3 &v1, const vec3 &v2)
{
	return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}
inline vec3 operator/(const vec3 &v, float t)
{
	return vec3(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}
inline vec3 &vec3::operator+=(const vec3 &v)
{
	e[0] += v.e[0];
	e[1] += v.e[1];
	e[2] += v.e[2];
	return *this;
}
inline vec3 &vec3::operator-=(const vec3 &v)
{
	e[0] -= v.e[0];
	e[1] -= v.e[1];
	e[2] -= v.e[2];
	return *this;
}
inline vec3 &vec3::operator*=(const float t)
{
	e[0] *= t;
	e[1] *= t;
	e[2] *= t;
	return *this;
}
inline vec3 &vec3::operator/=(const float t)
{
	e[0] /= t;
	e[1] /= t;
	e[2] /= t;
	return *this;
}

inline vec3 reflect(const vec3 &in, const vec3 &normal)
{
	// assumes unit length normal
	return in - normal * (2 * dot(in, normal));
}
inline bool refract(const vec3 &v, const vec3 &n, float ni_over_nt, vec3 &refracted)
{
	vec3 uv = unitVector(v);
	float dt = dot(uv, n);
	float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);
	if (discriminant > 0)
	{
		refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
		return true;
	}
	else
		return false;
}

#endif

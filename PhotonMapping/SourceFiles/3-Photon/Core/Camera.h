#pragma once
#pragma once
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Ray.h"
#include "Core/TimeClockRandom.h"

class camera
{
public:
	camera(vec3 lookfrom, vec3 viewD, vec3 up, float vfov, float aspt, float aprt, float fcsdst, float t0 = 0.0f, float t1 = 0.0f)
	{
		lens_radius = aprt / 2.0f;
		focus_dist = fcsdst;
		fov = vfov;
		aspect = aspt;
		vup = up;
		viewD.makeUnitVector();
		viewDirection = viewD;
		ovD = 1.0;
		theta = fov * Pi / 180.0;
		half_height = tan(theta / 2);
		half_width = aspect * half_height;
		origin = lookfrom;
		w = unitVector(viewDirection);
		u = unitVector(cross(vup, w));
		v = cross(w, u);
		lower_left_corner = origin - half_width * focus_dist * u - half_height * focus_dist * v + focus_dist * w;
		horizontal = 2 * focus_dist * half_width * u;
		vertical = 2 * focus_dist * half_height * v;
		if (viewD.x() != 0)
		{
			xAngle = reANGLE(atanf(viewD.y() / viewD.x()));
		}
		else
		{
			if (viewD.z() > 0)
				xAngle = -90.0;
			else if (viewD.z() < 0)
				xAngle = 90.0;
		}
		yAngle = 0.0f;
		time0 = t0;
		time1 = t1;
	}
	void setAspt(float aspt)
	{
		aspect = aspt;
		half_width = aspect * half_height;
		lower_left_corner = origin - half_width * focus_dist * u - half_height * focus_dist * v + focus_dist * w;
		horizontal = 2 * focus_dist * half_width * u;
		vertical = 2 * focus_dist * half_height * v;
	}
	Ray get_ray(float s, float t)
	{
		vec3 rd = lens_radius * random_in_unit_disk();
		vec3 offset = u * rd.x() + v * rd.y();
		float time = time0 + getClockRandom() * (time1 - time0);
		return Ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset, time);
	}
	void setDirection()
	{
		viewDirection[0] = ovD * cosf(ANGLE(yAngle)) * cosf(ANGLE(xAngle));
		viewDirection[2] = -ovD * cosf(ANGLE(yAngle)) * sinf(ANGLE(xAngle));
		viewDirection[1] = ovD * sinf(ANGLE(yAngle));
		w = unitVector(viewDirection);
		u = unitVector(cross(vup, w));
		v = cross(w, u);
		lower_left_corner = origin - half_width * focus_dist * u - half_height * focus_dist * v + focus_dist * w;
		horizontal = 2 * focus_dist * half_width * u;
		vertical = 2 * focus_dist * half_height * v;
	}
	void setPosition()
	{
		lower_left_corner = origin - half_width * focus_dist * u - half_height * focus_dist * v + focus_dist * w;
		horizontal = 2 * focus_dist * half_width * u;
		vertical = 2 * focus_dist * half_height * v;
	}
	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 viewDirection;
	vec3 origin;
	vec3 vup;
	vec3 u, v, w;
	float fov;
	float aspect;
	float theta;
	float half_height;
	float half_width;
	float xAngle;
	float yAngle;
	float lens_radius;
	float focus_dist;
	float ovD; // 视点到视平面的距离
	float time0, time1;
};

void setCameraDirection(float xBias, float yBias, camera &cam);
void setCameraPositionFront(float Bias, camera &cam);
void setCameraPositionBack(float Bias, camera &cam);
void setCameraPositionLeft(float Bias, camera &cam);
void setCameraPositionRight(float Bias, camera &cam);

extern camera cam;

#endif

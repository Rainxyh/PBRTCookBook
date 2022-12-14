#pragma once
#ifndef __PDF_H__
#define __PDF_H__

#include "Core/Vector.h"
#include "Core/TimeClockRandom.h"
#include "Core/ONB.h"
#include "Core/Hitable.h"

class pdf
{
public:
	virtual float value(const vec3 &direction) const = 0;
	virtual vec3 generate() const = 0;
};

class cosine_pdf : public pdf
{
public:
	cosine_pdf(const vec3 &w) { uvw.build_from_w(w); }
	virtual float value(const vec3 &direction) const
	{
		float cosine = dot(unitVector(direction), uvw.w());
		if (cosine > 0)
			return cosine / Pi;
		else
			return 0;
	}
	virtual vec3 generate() const
	{
		return uvw.local(random_cosine_direction());
	}
	onb uvw;
};

class hitable_pdf : public pdf
{
public:
	hitable_pdf(hitable *p, const vec3 &origin) : ptr(p), o(origin) {}
	virtual float value(const vec3 &direction) const
	{
		return ptr->pdf_value(o, direction);
	}

	virtual vec3 generate() const
	{
		return ptr->randomSample(o);
	}

	vec3 o;
	hitable *ptr;
};

class mixture_pdf : public pdf
{
public:
	mixture_pdf(pdf *p0, pdf *p1)
	{
		p[0] = p0;
		p[1] = p1;
	}
	virtual float value(const vec3 &direction) const
	{
		return 0.5f * p[0]->value(direction) + 0.5f * p[1]->value(direction);
	}

	virtual vec3 generate() const
	{
		if (getClockRandom() < 0.5f)
			return p[0]->generate();
		else
			return p[1]->generate();
	}
	pdf *p[2];
};

#endif

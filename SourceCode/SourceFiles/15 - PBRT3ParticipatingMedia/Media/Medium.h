#pragma once
#ifndef __Medium_h__
#define __Medium_h__

#include "Core\FeimosRender.h"
#include "Core\Spectrum.h"

namespace Feimos {

// Media Declarations
class PhaseFunction {
public:
	// PhaseFunction Interface
	virtual ~PhaseFunction() {}
	virtual float p(const Vector3f &wo, const Vector3f &wi) const = 0;
	virtual float Sample_p(const Vector3f &wo, Vector3f *wi,
		const Point2f &u) const = 0;
};

// Media Inline Functions
inline float PhaseHG(float cosTheta, float g) {
	float denom = 1 + g * g + 2 * g * cosTheta;
	return Inv4Pi * (1 - g * g) / (denom * std::sqrt(denom));
}

// HenyeyGreenstein Declarations
class HenyeyGreenstein : public PhaseFunction {
public:
	// HenyeyGreenstein Public Methods
	HenyeyGreenstein(float g) : g(g) {}
	float p(const Vector3f &wo, const Vector3f &wi) const;
	float Sample_p(const Vector3f &wo, Vector3f *wi,
		const Point2f &sample) const;
private:
	const float g;
};

// Medium Declarations
class Medium {
public:
	// Medium Interface
	virtual ~Medium() {}
	virtual Spectrum Tr(const Ray &ray, Sampler &sampler) const = 0;
	virtual Spectrum Sample(const Ray &ray, Sampler &sampler,
		MediumInteraction *mi) const = 0;
};

// MediumInterface Declarations
struct MediumInterface {
	MediumInterface() : inside(nullptr), outside(nullptr) {}
	// MediumInterface Public Methods
	MediumInterface(const Medium *medium) : inside(medium), outside(medium) {}
	MediumInterface(const Medium *inside, const Medium *outside)
		: inside(inside), outside(outside) {}
	bool IsMediumTransition() const { return inside != outside; }
	const Medium *inside, *outside; 
};



}



#endif









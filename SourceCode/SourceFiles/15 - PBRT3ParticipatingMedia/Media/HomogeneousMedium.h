#pragma once
#ifndef __HomogeneousMedium_h__
#define __HomogeneousMedium_h__

#include "Media/Medium.h"
#include "Core/FeimosRender.h"
#include "Core/Spectrum.h"


namespace Feimos {

	// HomogeneousMedium Declarations
	class HomogeneousMedium : public Medium {
	public:
		// HomogeneousMedium Public Methods
		HomogeneousMedium(const Spectrum &sigma_a, const Spectrum &sigma_s, float g)
			: sigma_a(sigma_a),
			sigma_s(sigma_s),
			sigma_t(sigma_s + sigma_a),
			g(g) {}
		Spectrum Tr(const Ray &ray, Sampler &sampler) const;
		Spectrum Sample(const Ray &ray, Sampler &sampler,
			MediumInteraction *mi) const;
	private:
		// HomogeneousMedium Private Data
		const Spectrum sigma_a, sigma_s, sigma_t;
		const float g;
	};


}



#endif



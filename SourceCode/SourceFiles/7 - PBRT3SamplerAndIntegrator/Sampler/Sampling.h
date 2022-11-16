#pragma once
#ifndef __sampling_h__
#define __sampling_h__

#include "Core\FeimosRender.h"
#include "Core\Geometry.hpp"
#include "Core\Transform.h"
#include "Sampler\RNG.h"
#include <vector>
#include <iostream>

namespace Feimos {



Point2f ConcentricSampleDisk(const Point2f &u);


// Sampling Inline Functions
template <typename T>
void Shuffle(T *samp, int count, int nDimensions, RNG &rng) {
	for (int i = 0; i < count; ++i) {
		int other = i + rng.UniformUInt32(count - i);
		for (int j = 0; j < nDimensions; ++j)
			std::swap(samp[nDimensions * i + j], samp[nDimensions * other + j]);
	}
}




}



#endif




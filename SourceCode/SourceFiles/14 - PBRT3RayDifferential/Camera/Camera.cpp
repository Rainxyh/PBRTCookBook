#include "Camera\Camera.h"

namespace Feimos{



float Camera::GenerateRayDifferential(const CameraSample &sample,
	RayDifferential *rd) const {
	float wt = GenerateRay(sample, rd);
	if (wt == 0) return 0;

	// Find camera ray after shifting a fraction of a pixel in the $x$ direction
	float wtx;
	for (float eps : { .05, -.05 }) {
		CameraSample sshift = sample;
		sshift.pFilm.x += eps;
		Ray rx;
		wtx = GenerateRay(sshift, &rx);
		rd->rxOrigin = rd->o + (rx.o - rd->o) / eps;
		rd->rxDirection = rd->d + (rx.d - rd->d) / eps;
		if (wtx != 0)
			break;
	}
	if (wtx == 0)
		return 0;

	// Find camera ray after shifting a fraction of a pixel in the $y$ direction
	float wty;
	for (float eps : { .05, -.05 }) {
		CameraSample sshift = sample;
		sshift.pFilm.y += eps;
		Ray ry;
		wty = GenerateRay(sshift, &ry);
		rd->ryOrigin = rd->o + (ry.o - rd->o) / eps;
		rd->ryDirection = rd->d + (ry.d - rd->d) / eps;
		if (wty != 0)
			break;
	}
	if (wty == 0)
		return 0;

	rd->hasDifferentials = true;
	return wt;
}



}






#pragma once
#include "Camera\Camera.h"


namespace Feimos {

// OrthographicCamera Declarations
class OrthographicCamera : public ProjectiveCamera {
  public:
    // OrthographicCamera Public Methods
    OrthographicCamera(const int RasterWidth, const int RasterHeight, const Transform &CameraToWorld,
                       const Bounds2f &screenWindow, float lensRadius,
                       float focalDistance)
        : ProjectiveCamera(RasterWidth, RasterHeight, CameraToWorld, Orthographic(0, 10), screenWindow, lensRadius, focalDistance) {
		// Compute differential changes in origin for orthographic camera rays
		dxCamera = RasterToCamera(Vector3f(1, 0, 0));
		dyCamera = RasterToCamera(Vector3f(0, 1, 0));
    }
    float GenerateRay(const CameraSample &sample, Ray *) const;
	float GenerateRayDifferential(const CameraSample &sample,
		RayDifferential *) const;
private:
	// OrthographicCamera Private Data
	Vector3f dxCamera, dyCamera;
};






}






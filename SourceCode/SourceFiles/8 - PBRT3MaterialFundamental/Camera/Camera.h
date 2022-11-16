#pragma once
#pragma once
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Sampler\TimeClockRandom.h"
#include "Core\FeimosRender.h"
#include "Core\Geometry.hpp"
#include "Core\Transform.h"

namespace Feimos {

struct CameraSample {
    Point2f pFilm;
    Point2f pLens;
	float time;
};

class Camera {
public:
    // Camera Interface
	Camera(const Transform &CameraToWorld):CameraToWorld(CameraToWorld){}
	virtual ~Camera() {}
	virtual float GenerateRay(const CameraSample &sample, Ray *ray) const { return 1; };

    // Camera Public Data
	Transform CameraToWorld;
};

class ProjectiveCamera : public Camera {
public:
	// ProjectiveCamera Public Methods
	ProjectiveCamera(const int RasterWidth, const int RasterHeight, const Transform &CameraToWorld,
		const Transform &CameraToScreen,
		const Bounds2f &screenWindow, float lensr, float focald)
		: Camera(CameraToWorld),
		CameraToScreen(CameraToScreen) {
		// Initialize depth of field parameters
		lensRadius = lensr;
		focalDistance = focald;
		// Compute projective camera screen transformations
		ScreenToRaster =
			Scale(RasterWidth, RasterHeight, 1) *
			Scale(1 / (screenWindow.pMax.x - screenWindow.pMin.x),
				1 / (screenWindow.pMin.y - screenWindow.pMax.y), 1) *
			Translate(Vector3f(-screenWindow.pMin.x, -screenWindow.pMax.y, 0));
		RasterToScreen = Inverse(ScreenToRaster);
		RasterToCamera = Inverse(CameraToScreen) * RasterToScreen;
	}

protected:
	// ProjectiveCamera Protected Data
	Transform CameraToScreen, RasterToCamera;
	Transform ScreenToRaster, RasterToScreen;
	float lensRadius, focalDistance;
};



PerspectiveCamera *CreatePerspectiveCamera(const int RasterWidth, const int RasterHeight, const Transform &cam2world);
OrthographicCamera *CreateOrthographicCamera(const int RasterWidth, const int RasterHeight, const Transform &cam2world);




}

#endif








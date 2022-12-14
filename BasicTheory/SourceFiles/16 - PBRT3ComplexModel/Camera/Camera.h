#pragma once
#pragma once
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Sampler/TimeClockRandom.h"
#include "Core/FeimosRender.h"
#include "Core/Geometry.h"
#include "Core/Transform.h"

namespace Feimos
{

	struct CameraSample
	{
		Point2f pFilm;
		Point2f pLens;
		float time;
	};

	class Camera
	{
	public:
		// Camera Interface
		Camera(const Transform &CameraToWorld, const Medium *medium = nullptr) : CameraToWorld(CameraToWorld), medium(medium) {}
		virtual ~Camera() {}
		virtual float GenerateRay(const CameraSample &sample, Ray *ray) const { return 1; };
		virtual float GenerateRayDifferential(const CameraSample &sample, RayDifferential *rd) const;
		// Camera Public Data
		Transform CameraToWorld;
		const Medium *medium;
	};

	class ProjectiveCamera : public Camera
	{
	public:
		// ProjectiveCamera Public Methods
		ProjectiveCamera(const int RasterWidth, const int RasterHeight, const Transform &CameraToWorld,
						 const Transform &CameraToScreen, // 直接调用Perspective(fov, 1e-2f, 1000.f)透视变换矩阵(相机空间->
						 const Bounds2f &screenWindow, float lensr, float focald,
						 const Medium *medium)
			: Camera(CameraToWorld, medium),
			  CameraToScreen(CameraToScreen)
		{
			// Initialize depth of field parameters
			lensRadius = lensr;
			focalDistance = focald;
			// Compute projective camera screen transformations
			ScreenToRaster =
				Scale(RasterWidth, RasterHeight, 1) *
				Scale(1 / (screenWindow.pMax.x - screenWindow.pMin.x),
					  1 / (screenWindow.pMin.y - screenWindow.pMax.y), 1) *
				Translate(Vector3f(-screenWindow.pMin.x, -screenWindow.pMax.y, 0));  // 屏幕空间->光栅空间  screenWindow(短边[-1,1],长边进行相应缩放)并非NDC(均映射至[-1,1])->设定的图片分辨率大小
			RasterToScreen = Inverse(ScreenToRaster);  // 光栅空间->屏幕空间
			RasterToCamera = Inverse(CameraToScreen) * RasterToScreen;  // 光栅空间->屏幕空间->相机空间
		}

	protected:
		// ProjectiveCamera Protected Data
		Transform CameraToScreen, RasterToCamera;
		Transform ScreenToRaster, RasterToScreen;
		float lensRadius, focalDistance;
	};

	PerspectiveCamera *CreatePerspectiveCamera(const int RasterWidth, const int RasterHeight, const Transform &cam2world, Medium *media = nullptr);
	OrthographicCamera *CreateOrthographicCamera(const int RasterWidth, const int RasterHeight, const Transform &cam2world, Medium *media = nullptr);

}

#endif

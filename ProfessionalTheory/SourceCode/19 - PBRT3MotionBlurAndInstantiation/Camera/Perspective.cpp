#include "Camera/Perspective.h"
#include "Sampler/Sampling.h"

namespace Feimos
{

	PerspectiveCamera::PerspectiveCamera(const int RasterWidth, const int RasterHeight, const AnimatedTransform &CameraToWorld,
										 const Bounds2f &screenWindow, float shutterOpen, float shutterClose, float lensRadius, float focalDistance, float fov, const Medium *medium)
		: ProjectiveCamera(RasterWidth, RasterHeight, CameraToWorld, Perspective(fov, 1e-2f, 1000.f), screenWindow, shutterOpen, shutterClose, lensRadius, focalDistance, medium)
	{

		// Compute differential changes in origin for perspective camera rays
		dxCamera =
			(RasterToCamera(Point3f(1, 0, 0)) - RasterToCamera(Point3f(0, 0, 0)));
		dyCamera =
			(RasterToCamera(Point3f(0, 1, 0)) - RasterToCamera(Point3f(0, 0, 0)));

		// Compute image plane bounds at $z=1$ for _PerspectiveCamera_
		Point2i res = Point2i(RasterWidth, RasterHeight);
		Point3f pMin = RasterToCamera(Point3f(0, 0, 0));
		Point3f pMax = RasterToCamera(Point3f(res.x, res.y, 0));
		pMin /= pMin.z;
		pMax /= pMax.z;
	}

	float PerspectiveCamera::GenerateRay(const CameraSample &sample, Ray *ray) const
	{
		// Compute raster and camera sample positions
		Point3f pFilm = Point3f(sample.pFilm.x, sample.pFilm.y, 0);
		Point3f pCamera = RasterToCamera(pFilm);
		*ray = Ray(Point3f(0, 0, 0), Normalize(Vector3f(pCamera)));
		// Modify ray for depth of field
		if (lensRadius > 0)
		{
			// Sample point on lens
			Point2f pLens = lensRadius * ConcentricSampleDisk(sample.pLens);

			// Compute point on plane of focus
			float ft = focalDistance / ray->d.z;
			Point3f pFocus = (*ray)(ft);

			// Update ray for effect of lens
			ray->o = Point3f(pLens.x, pLens.y, 0);
			ray->d = Normalize(pFocus - ray->o);
		}
		ray->time = Lerp(sample.time, shutterOpen, shutterClose);
		ray->medium = medium;
		*ray = CameraToWorld(*ray);
		return 1;
	}

	float PerspectiveCamera::GenerateRayDifferential(const CameraSample &sample, RayDifferential *ray) const
	{
		// Compute raster and camera sample positions
		Point3f pFilm = Point3f(sample.pFilm.x, sample.pFilm.y, 0);
		Point3f pCamera = RasterToCamera(pFilm);
		Vector3f dir = Normalize(Vector3f(pCamera.x, pCamera.y, pCamera.z));
		*ray = RayDifferential(Point3f(0, 0, 0), dir);
		// Modify ray for depth of field
		if (lensRadius > 0)
		{
			// Sample point on lens
			Point2f pLens = lensRadius * ConcentricSampleDisk(sample.pLens);

			// Compute point on plane of focus
			float ft = focalDistance / ray->d.z;
			Point3f pFocus = (*ray)(ft);

			// Update ray for effect of lens
			ray->o = Point3f(pLens.x, pLens.y, 0);
			ray->d = Normalize(pFocus - ray->o);
		}

		// Compute offset rays for _PerspectiveCamera_ ray differentials
		if (lensRadius > 0)
		{
			// Compute _PerspectiveCamera_ ray differentials accounting for lens

			// Sample point on lens
			Point2f pLens = lensRadius * ConcentricSampleDisk(sample.pLens);
			Vector3f dx = Normalize(Vector3f(pCamera + dxCamera));
			float ft = focalDistance / dx.z;
			Point3f pFocus = Point3f(0, 0, 0) + (ft * dx);
			ray->rxOrigin = Point3f(pLens.x, pLens.y, 0);
			ray->rxDirection = Normalize(pFocus - ray->rxOrigin);

			Vector3f dy = Normalize(Vector3f(pCamera + dyCamera));
			ft = focalDistance / dy.z;
			pFocus = Point3f(0, 0, 0) + (ft * dy);
			ray->ryOrigin = Point3f(pLens.x, pLens.y, 0);
			ray->ryDirection = Normalize(pFocus - ray->ryOrigin);
		}
		else
		{
			ray->rxOrigin = ray->ryOrigin = ray->o;
			ray->rxDirection = Normalize(Vector3f(pCamera) + dxCamera);
			ray->ryDirection = Normalize(Vector3f(pCamera) + dyCamera);
		}
		ray->time = Lerp(sample.time, shutterOpen, shutterClose);
		ray->medium = medium;
		*ray = CameraToWorld(*ray);
		ray->hasDifferentials = true;
		return 1;
	}

	PerspectiveCamera *CreatePerspectiveCamera(const int RasterWidth, const int RasterHeight, const AnimatedTransform &cam2world, float shutterOpen, float shutterClose, Medium *media)
	{
		float lensradius = 0.02f;
		float focaldistance = 3.0f;
		float frame = (float)RasterWidth / (float)RasterHeight;
		Bounds2f screen;
		if (frame > 1.f)
		{
			screen.pMin.x = -frame;
			screen.pMax.x = frame;
			screen.pMin.y = -1.f;
			screen.pMax.y = 1.f;
		}
		else
		{
			screen.pMin.x = -1.f;
			screen.pMax.x = 1.f;
			screen.pMin.y = -1.f / frame;
			screen.pMax.y = 1.f / frame;
		}
		float fov = 90.0f;
		float halffov = 45.0f;
		return new PerspectiveCamera(RasterWidth, RasterHeight, cam2world, screen, shutterOpen, shutterClose, lensradius, focaldistance, fov, media);
	}

}

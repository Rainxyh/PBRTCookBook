#pragma once
#include "Camera/Camera.h"

namespace Feimos
{

  // OrthographicCamera Declarations
  class OrthographicCamera : public ProjectiveCamera
  {
  public:
    // OrthographicCamera Public Methods
    OrthographicCamera(const int RasterWidth, const int RasterHeight, const AnimatedTransform &CameraToWorld,
                       const Bounds2f &screenWindow, float shutterOpen, float shutterClose,
                       float lensRadius, float focalDistance, const Medium *medium)
        : ProjectiveCamera(RasterWidth, RasterHeight, CameraToWorld, Orthographic(0, 10), screenWindow, shutterOpen, shutterClose, lensRadius, focalDistance, medium)
    {
      // Compute differential changes in origin for orthographic camera rays
      dxCamera = RasterToCamera(Vector3f(1, 0, 0));
      dyCamera = RasterToCamera(Vector3f(0, 1, 0));
    }
    float GenerateRay(const CameraSample &sample, Ray *) const;
    float GenerateRayDifferential(const CameraSample &sample, RayDifferential *) const;

  private:
    // OrthographicCamera Private Data
    Vector3f dxCamera, dyCamera;
  };

}

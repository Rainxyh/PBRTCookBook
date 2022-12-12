#pragma once
#include "Camera/Camera.h"

namespace Feimos
{

  // OrthographicCamera Declarations
  class OrthographicCamera : public ProjectiveCamera
  {
  public:
    // OrthographicCamera Public Methods
    OrthographicCamera(const int RasterWidth, const int RasterHeight, const Transform &CameraToWorld,
                       const Bounds2f &screenWindow, float lensRadius,
                       float focalDistance);
    float GenerateRay(const CameraSample &sample, Ray *) const;
  };

}

#pragma once
#include "Camera/Camera.h"


namespace Feimos {

class PerspectiveCamera : public ProjectiveCamera {
  public:
    // PerspectiveCamera Public Methods
    PerspectiveCamera(const int RasterWidth, const int RasterHeight, const Transform &CameraToWorld,
                      const Bounds2f &screenWindow, float lensRadius, float focalDistance,
                      float fov);
    float GenerateRay(const CameraSample &sample, Ray *) const;
};




}




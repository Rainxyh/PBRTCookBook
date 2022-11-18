#pragma once
#pragma once
#ifndef __SPHEREH__
#define __SPHEREH__

#include "Core/FeimosRender.h"
#include "Shape/Shape.h"

namespace Feimos
{

  // Sphere Declarations
  class Sphere : public Shape
  {
  public:
    // Sphere Public Methods
    Sphere(const Transform *ObjectToWorld, const Transform *WorldToObject,
           bool reverseOrientation, float radius)
        : Shape(ObjectToWorld, WorldToObject, reverseOrientation),
          radius(radius) {}
    Bounds3f ObjectBound() const;
    bool Intersect(const Ray &ray, float *tHit, SurfaceInteraction *isect,
                   bool testAlphaTexture) const;
    bool IntersectP(const Ray &ray, bool testAlphaTexture) const;
    float Area() const;

  private:
    // Sphere Private Data
    const float radius;
  };

}

#endif

#pragma once
#ifndef __TEXTURE_H__
#define __TEXTURE_H__
#include "Core\Geometry.hpp"
#include "Core\FeimosRender.h"

namespace Feimos{


template <typename T>
class Texture {
  public:
    // Texture Interface
    virtual T Evaluate(const SurfaceInteraction &) const = 0;
    virtual ~Texture() {}
};




}






#endif








#pragma once
#ifndef __ConstantTexture_h__
#define __ConstantTexture_h__

#include "Texture/Texture.h"

namespace Feimos {

template <typename T>
class ConstantTexture : public Texture<T> {
  public:
    // ConstantTexture Public Methods
    ConstantTexture(const T &value) : value(value) {}
    T Evaluate(const SurfaceInteraction &) const { return value; }

  private:
    T value;
};

}







#endif








#pragma once
#ifndef _MATERIAL_H__
#define _MATERIAL_H__

#include "Core/FeimosRender.h"

namespace Feimos {
// TransportMode Declarations
enum class TransportMode { Radiance, Importance };

// Material Declarations
class Material {
  public:
    // Material Interface
    virtual void ComputeScatteringFunctions(SurfaceInteraction *si,
                                            TransportMode mode,
                                            bool allowMultipleLobes) const = 0;
    virtual ~Material() {}
};

}





#endif









#pragma once
#ifndef __GlassMaterial_h__
#define __GlassMaterial_h__

#include "Material/Material.h"
#include "Core/FeimosRender.h"
#include "Texture/Texture.h"

namespace Feimos {

// GlassMaterial Declarations
class GlassMaterial : public Material {
  public:
    // GlassMaterial Public Methods
    GlassMaterial(const std::shared_ptr<Texture<Spectrum>> &Kr,
                  const std::shared_ptr<Texture<Spectrum>> &Kt,
                  const std::shared_ptr<Texture<float>> &uRoughness,
                  const std::shared_ptr<Texture<float>> &vRoughness,
                  const std::shared_ptr<Texture<float>> &index,
                  const std::shared_ptr<Texture<float>> &bumpMap,
                  bool remapRoughness)
        : Kr(Kr),
          Kt(Kt),
          uRoughness(uRoughness),
          vRoughness(vRoughness),
          index(index),
          bumpMap(bumpMap),
          remapRoughness(remapRoughness) {}
    void ComputeScatteringFunctions(SurfaceInteraction *si, 
                                    TransportMode mode,
                                    bool allowMultipleLobes) const;

  private:
    // GlassMaterial Private Data
    std::shared_ptr<Texture<Spectrum>> Kr, Kt;
    std::shared_ptr<Texture<float>> uRoughness, vRoughness;
    std::shared_ptr<Texture<float>> index;
    std::shared_ptr<Texture<float>> bumpMap;
    bool remapRoughness;
};

}







#endif






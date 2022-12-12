#pragma once
#ifndef __MetalMaterial_h__
#define __MetalMaterial_h__
#include "Material/Material.h"
#include "Core/FeimosRender.h"
#include "Texture/Texture.h"
namespace Feimos
{

  // MetalMaterial Declarations
  class MetalMaterial : public Material
  {
  public:
    // MetalMaterial Public Methods
    MetalMaterial(const std::shared_ptr<Texture<Spectrum>> &eta,
                  const std::shared_ptr<Texture<Spectrum>> &k,
                  const std::shared_ptr<Texture<float>> &rough,
                  const std::shared_ptr<Texture<float>> &urough,
                  const std::shared_ptr<Texture<float>> &vrough,
                  const std::shared_ptr<Texture<float>> &bump,
                  bool remapRoughness);
    void ComputeScatteringFunctions(SurfaceInteraction *si,
                                    TransportMode mode, bool allowMultipleLobes) const;

  private:
    // MetalMaterial Private Data
    std::shared_ptr<Texture<Spectrum>> eta, k;
    std::shared_ptr<Texture<float>> roughness, uRoughness, vRoughness;
    std::shared_ptr<Texture<float>> bumpMap;
    bool remapRoughness;
  };

}

#endif

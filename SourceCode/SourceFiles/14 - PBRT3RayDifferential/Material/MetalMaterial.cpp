#include "Material/Material.h"
#include "Material/MetalMaterial.h"
#include "Material/Reflection.h"
#include "Material/Microfacet.h"


namespace Feimos {

// MetalMaterial Method Definitions
MetalMaterial::MetalMaterial(const std::shared_ptr<Texture<Spectrum>> &eta,
                             const std::shared_ptr<Texture<Spectrum>> &k,
                             const std::shared_ptr<Texture<float>> &roughness,
                             const std::shared_ptr<Texture<float>> &uRoughness,
                             const std::shared_ptr<Texture<float>> &vRoughness,
                             const std::shared_ptr<Texture<float>> &bumpMap,
                             bool remapRoughness)
    : eta(eta),
      k(k),
      roughness(roughness),
      uRoughness(uRoughness),
      vRoughness(vRoughness),
      bumpMap(bumpMap),
      remapRoughness(remapRoughness) {}

void MetalMaterial::ComputeScatteringFunctions(SurfaceInteraction *si,
                                               TransportMode mode,
                                               bool allowMultipleLobes) const {
    // Perform bump mapping with _bumpMap_, if present
    //if (bumpMap) Bump(bumpMap, si);
	si->bsdf = std::make_shared<BSDF>(*si);

    float uRough =
        uRoughness ? uRoughness->Evaluate(*si) : roughness->Evaluate(*si);
    float vRough =
        vRoughness ? vRoughness->Evaluate(*si) : roughness->Evaluate(*si);
    if (remapRoughness) {
        uRough = TrowbridgeReitzDistribution::RoughnessToAlpha(uRough);
        vRough = TrowbridgeReitzDistribution::RoughnessToAlpha(vRough);
    }
    Fresnel *frMf = new FresnelConductor(1., eta->Evaluate(*si),
                                                         k->Evaluate(*si));
    MicrofacetDistribution *distrib = new TrowbridgeReitzDistribution(uRough, vRough);
    si->bsdf->Add(new MicrofacetReflection(1., distrib, frMf));
}

}

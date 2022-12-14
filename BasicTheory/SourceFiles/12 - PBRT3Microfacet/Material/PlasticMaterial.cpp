#include "Material/PlasticMaterial.h"
#include "Material/Reflection.h"
#include "Material/Microfacet.h"

namespace Feimos
{

    // PlasticMaterial Method Definitions
    void PlasticMaterial::ComputeScatteringFunctions(SurfaceInteraction *si, TransportMode mode,
                                                     bool allowMultipleLobes) const
    {
        // Perform bump mapping with _bumpMap_, if present
        // if (bumpMap) Bump(bumpMap, si);
        si->bsdf = std::make_shared<BSDF>(*si);
        // Initialize diffuse component of plastic material
        Spectrum kd = Kd->Evaluate(*si).Clamp();
        if (!kd.IsBlack())
            si->bsdf->Add(new LambertianReflection(kd));

        // Initialize specular component of plastic material
        Spectrum ks = Ks->Evaluate(*si).Clamp();
        if (!ks.IsBlack())
        {
            Fresnel *fresnel = new FresnelDielectric(1.5f, 1.f);
            // Create microfacet distribution _distrib_ for plastic material
            float rough = roughness->Evaluate(*si);
            if (remapRoughness)
                rough = TrowbridgeReitzDistribution::RoughnessToAlpha(rough);
            MicrofacetDistribution *distrib = new TrowbridgeReitzDistribution(rough, rough);
            BxDF *spec = new MicrofacetReflection(ks, distrib, fresnel);
            si->bsdf->Add(spec);
        }
    }

}

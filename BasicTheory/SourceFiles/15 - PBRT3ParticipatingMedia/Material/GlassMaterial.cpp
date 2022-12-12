#include "Material/GlassMaterial.h"
#include "Material/Reflection.h"
#include "Material/Microfacet.h"

namespace Feimos
{

    // GlassMaterial Method Definitions
    void GlassMaterial::ComputeScatteringFunctions(SurfaceInteraction *si,
                                                   TransportMode mode,
                                                   bool allowMultipleLobes) const
    {
        // Perform bump mapping with _bumpMap_, if present
        // if (bumpMap) Bump(bumpMap, si);
        float eta = index->Evaluate(*si);
        float urough = uRoughness->Evaluate(*si);
        float vrough = vRoughness->Evaluate(*si);
        Spectrum R = Kr->Evaluate(*si).Clamp();
        Spectrum T = Kt->Evaluate(*si).Clamp();
        // Initialize _bsdf_ for smooth or rough dielectric
        si->bsdf = std::make_shared<BSDF>(*si, eta);

        if (R.IsBlack() && T.IsBlack())
            return;

        bool isSpecular = urough == 0 && vrough == 0;
        if (isSpecular && allowMultipleLobes)
        {
            si->bsdf->Add(new FresnelSpecular(R, T, 1.f, eta, mode));
        }
        else
        {
            if (remapRoughness)
            {
                urough = TrowbridgeReitzDistribution::RoughnessToAlpha(urough);
                vrough = TrowbridgeReitzDistribution::RoughnessToAlpha(vrough);
            }

            if (!R.IsBlack())
            {
                Fresnel *fresnel = new FresnelDielectric(1.f, eta);
                if (isSpecular)
                    si->bsdf->Add(new SpecularReflection(R, fresnel));
                else
                {
                    MicrofacetDistribution *distrib =
                        isSpecular ? nullptr
                                   : new TrowbridgeReitzDistribution(urough, vrough);
                    si->bsdf->Add(new MicrofacetReflection(R, distrib, fresnel));
                }
            }
            if (!T.IsBlack())
            {
                if (isSpecular)
                    si->bsdf->Add(new SpecularTransmission(T, 1.f, eta, mode));
                else
                {
                    MicrofacetDistribution *distrib =
                        isSpecular ? nullptr
                                   : new TrowbridgeReitzDistribution(urough, vrough);
                    si->bsdf->Add(new MicrofacetTransmission(T, distrib, 1.f, eta, mode));
                }
            }
        }
    }

}

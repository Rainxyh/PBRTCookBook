#include "Core/FeimosRender.h"

#include "Material/MatteMaterial.h"
#include "Material/PerfectGlassMaterial.h"
#include "Material/Reflection.h"

#include "Core/Spectrum.h"
#include "Core/interaction.h"

namespace Feimos
{

    // PerfectGlassMaterial Method Definitions
    void PerfectGlassMaterial::ComputeScatteringFunctions(SurfaceInteraction *si,
                                                          TransportMode mode,
                                                          bool allowMultipleLobes) const
    {
        // Evaluate textures for _PerfectGlassMaterial_ material and allocate BRDF
        si->bsdf = std::make_shared<BSDF>(*si);

        float eta = index->Evaluate(*si);
        Spectrum R = Kr->Evaluate(*si).Clamp(), T = Kt->Evaluate(*si).Clamp();
        if (!R.IsBlack() && !T.IsBlack())
        {
            si->bsdf->Add(new FresnelSpecular(R, T, 1.f, eta, mode));
        }
    }

}

#pragma once
#include "Material/Material.h"
#include "Texture/Texture.h"

namespace Feimos
{

    // PerfectGlassMaterial Declarations
    class PerfectGlassMaterial : public Material
    {
    public:
        // PerfectGlassMaterial Public Methods
        PerfectGlassMaterial(const std::shared_ptr<Texture<Spectrum>> &Kr,
                             const std::shared_ptr<Texture<Spectrum>> &Kt,
                             const std::shared_ptr<Texture<float>> &index,
                             const std::shared_ptr<Texture<float>> &bumpMap)
            : Kr(Kr), Kt(Kt), index(index), bumpMap(bumpMap) {}
        void ComputeScatteringFunctions(SurfaceInteraction *si,
                                        TransportMode mode,
                                        bool allowMultipleLobes) const;

    private:
        // PerfectGlassMaterial Private Data
        std::shared_ptr<Texture<Spectrum>> Kr, Kt;
        std::shared_ptr<Texture<float>> index, bumpMap;
    };

}
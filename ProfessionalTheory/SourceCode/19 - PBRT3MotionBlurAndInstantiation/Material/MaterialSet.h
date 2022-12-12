#ifndef __MaterialSet_h__
#define __MaterialSet_h__

#include "Material/Material.h"
#include "Material/MatteMaterial.h"
#include "Material/MetalMaterial.h"
#include "Material/PlasticMaterial.h"
#include "Material/GlassMaterial.h"
#include "Material/Mirror.h"

#include "Texture/Texture.h"
#include "Texture/ConstantTexture.h"
#include "Texture/ImageTexture.h"

namespace Feimos
{

	inline std::shared_ptr<Material> getPlasticMaterial(
		std::shared_ptr<Texture<Spectrum>> Kt = std::make_shared<ConstantTexture<Spectrum>>(Spectrum(0.75f)),
		bool trilerp = false, float maxAniso = 8.f, float scale = 1.f, bool gamma = false,
		std::shared_ptr<Texture<float>> plasticRoughness = std::make_shared<ConstantTexture<float>>(0.1f),
		std::shared_ptr<Texture<float>> bumpMap = std::make_shared<ConstantTexture<float>>(0.0f),
		ImageWrap wrapMode = ImageWrap::Repeat)
	{
		return std::make_shared<PlasticMaterial>(Kt, Kt, plasticRoughness, bumpMap, true);
	}

	inline std::shared_ptr<Feimos::Material> getMatteMaterial(
		std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> Kd = std::make_shared<ConstantTexture<Spectrum>>(Spectrum(0.75f)),
		std::shared_ptr<Feimos::Texture<float>> sigma = std::make_shared<Feimos::ConstantTexture<float>>(0.0f),
		std::shared_ptr<Feimos::Texture<float>> bumpMap = std::make_shared<Feimos::ConstantTexture<float>>(0.0f))
	{
		// 材质
		return std::make_shared<Feimos::MatteMaterial>(Kd, sigma, bumpMap);
	}

	inline std::shared_ptr<Feimos::Material> getMetalMaterial(
		std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> etaM = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(Spectrum(0.2f)),
		std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> kM = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(Spectrum(0.11f)),
		std::shared_ptr<Feimos::Texture<float>> Roughness = std::make_shared<Feimos::ConstantTexture<float>>(0.15f),
		std::shared_ptr<Feimos::Texture<float>> RoughnessU = std::make_shared<Feimos::ConstantTexture<float>>(0.15f),
		std::shared_ptr<Feimos::Texture<float>> RoughnessV = std::make_shared<Feimos::ConstantTexture<float>>(0.15f),
		std::shared_ptr<Feimos::Texture<float>> bumpMap = std::make_shared<Feimos::ConstantTexture<float>>(0.0f))
	{
		return std::make_shared<Feimos::MetalMaterial>(etaM, kM, Roughness, RoughnessU, RoughnessV, bumpMap, false);
	}

	inline std::shared_ptr<Feimos::Material> getGlassMaterial(
		std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> Kr = std::make_shared<Feimos::ConstantTexture<Spectrum>>(Spectrum(0.98)),
		std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> Kt = std::make_shared<Feimos::ConstantTexture<Spectrum>>(Spectrum(0.98)),
		std::shared_ptr<Feimos::Texture<float>> index = std::make_shared<Feimos::ConstantTexture<float>>(1.5f),
		std::shared_ptr<Feimos::Texture<float>> RoughnessU = std::make_shared<Feimos::ConstantTexture<float>>(0.1f),
		std::shared_ptr<Feimos::Texture<float>> RoughnessV = std::make_shared<Feimos::ConstantTexture<float>>(0.1f),
		std::shared_ptr<Feimos::Texture<float>> bumpMap = std::make_shared<Feimos::ConstantTexture<float>>(0.0f))
	{
		return std::make_shared<Feimos::GlassMaterial>(Kr, Kt,
													   RoughnessU, RoughnessV, index, bumpMap, false);
	}

	inline std::shared_ptr<Texture<Spectrum>> getImageTexture(const std::string &filename)
	{
		std::unique_ptr<Feimos::TextureMapping2D> map = std::make_unique<Feimos::UVMapping2D>(1.f, 1.f, 0.f, 0.f);
		Feimos::ImageWrap wrapMode = Feimos::ImageWrap::Repeat;
		bool trilerp = false;
		float maxAniso = 8.f;
		float scale = 1.f;
		bool gamma = false; // 如果是tga和png就是true;
		std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> Kt =
			std::make_shared<Feimos::ImageTexture<Feimos::RGBSpectrum, Feimos::Spectrum>>(std::move(map), filename, trilerp, maxAniso, wrapMode, scale, gamma);
		return Kt;
	}

}

#endif

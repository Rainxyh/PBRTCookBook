#pragma once
#ifndef __Reflection_h__
#define __Reflection_h__

#include "Core/FeimosRender.h"
#include "Core/Geometry.hpp"
#include "Core/Spectrum.h"
#include "Core/interaction.h"

#include "Material/Fresnel.h"
#include "Material/Microfacet.h"

#include <algorithm>
#include <string>

namespace Feimos
{

	// BSDF Inline Functions
	inline float CosTheta(const Vector3f &w) { return w.z; }
	inline float Cos2Theta(const Vector3f &w) { return w.z * w.z; }
	inline float AbsCosTheta(const Vector3f &w) { return std::abs(w.z); }
	inline float Sin2Theta(const Vector3f &w) { return std::max((float)0, (float)1 - Cos2Theta(w)); }
	inline float SinTheta(const Vector3f &w) { return std::sqrt(Sin2Theta(w)); }
	inline float TanTheta(const Vector3f &w) { return SinTheta(w) / CosTheta(w); }
	inline float Tan2Theta(const Vector3f &w) { return Sin2Theta(w) / Cos2Theta(w); }

	inline float CosPhi(const Vector3f &w)
	{
		float sinTheta = SinTheta(w);
		return (sinTheta == 0) ? 1 : Clamp(w.x / sinTheta, -1, 1);
	}
	inline float SinPhi(const Vector3f &w)
	{
		float sinTheta = SinTheta(w);
		return (sinTheta == 0) ? 0 : Clamp(w.y / sinTheta, -1, 1);
	}

	inline float Cos2Phi(const Vector3f &w) { return CosPhi(w) * CosPhi(w); }
	inline float Sin2Phi(const Vector3f &w) { return SinPhi(w) * SinPhi(w); }

	inline float CosDPhi(const Vector3f &wa, const Vector3f &wb)
	{
		float waxy = wa.x * wa.x + wa.y * wa.y;
		float wbxy = wb.x * wb.x + wb.y * wb.y;
		if (waxy == 0 || wbxy == 0)
			return 1;
		return Clamp((wa.x * wb.x + wa.y * wb.y) / std::sqrt(waxy * wbxy), -1, 1);
	}

	inline Vector3f Reflect(const Vector3f &wo, const Vector3f &n) { return -wo + 2 * Dot(wo, n) * n; }

	inline bool Refract(const Vector3f &wi, const Normal3f &n, float eta,
						Vector3f *wt)
	{
		// Compute $\cos \theta_\roman{t}$ using Snell's law
		float cosThetaI = Dot(n, wi);
		float sin2ThetaI = std::max(float(0), float(1 - cosThetaI * cosThetaI));
		float sin2ThetaT = eta * eta * sin2ThetaI;

		// Handle total internal reflection for transmission
		if (sin2ThetaT >= 1)
			return false;
		float cosThetaT = std::sqrt(1 - sin2ThetaT);
		*wt = eta * -wi + (eta * cosThetaI - cosThetaT) * Vector3f(n);
		return true;
	}

	inline bool SameHemisphere(const Vector3f &w, const Vector3f &wp) { return w.z * wp.z > 0; }

	inline bool SameHemisphere(const Vector3f &w, const Normal3f &wp) { return w.z * wp.z > 0; }

	// BSDF Declarations
	enum BxDFType
	{
		BSDF_REFLECTION = 1 << 0,
		BSDF_TRANSMISSION = 1 << 1,
		BSDF_DIFFUSE = 1 << 2,
		BSDF_GLOSSY = 1 << 3,
		BSDF_SPECULAR = 1 << 4,
		BSDF_ALL = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR | BSDF_REFLECTION |
				   BSDF_TRANSMISSION,
	};

	class BSDF
	{
	public:
		// BSDF Public Methods
		BSDF(const SurfaceInteraction &si, float eta = 1)
			: eta(eta),
			  ns(si.shading.n),
			  ng(si.n),
			  ss(Normalize(si.shading.dpdu)),
			  ts(Cross(ns, ss)) {}
		void Add(BxDF *b)
		{
			// CHECK_LT(nBxDFs, MaxBxDFs);
			bxdfs.push_back(std::shared_ptr<BxDF>(b));
			nBxDFs++;
		}
		int NumComponents(BxDFType flags = BSDF_ALL) const;
		Vector3f WorldToLocal(const Vector3f &v) const
		{
			return Vector3f(Dot(v, ss), Dot(v, ts), Dot(v, ns));
		}
		Vector3f LocalToWorld(const Vector3f &v) const
		{
			return Vector3f(ss.x * v.x + ts.x * v.y + ns.x * v.z,
							ss.y * v.x + ts.y * v.y + ns.y * v.z,
							ss.z * v.x + ts.z * v.y + ns.z * v.z);
		}
		Spectrum f(const Vector3f &woW, const Vector3f &wiW,
				   BxDFType flags = BSDF_ALL) const;
		Spectrum rho(int nSamples, const Point2f *samples1, const Point2f *samples2,
					 BxDFType flags = BSDF_ALL) const;
		Spectrum rho(const Vector3f &wo, int nSamples, const Point2f *samples,
					 BxDFType flags = BSDF_ALL) const;
		Spectrum Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &u,
						  float *pdf, BxDFType type = BSDF_ALL,
						  BxDFType *sampledType = nullptr) const;
		float Pdf(const Vector3f &wo, const Vector3f &wi,
				  BxDFType flags = BSDF_ALL) const;

		// BSDF Public Data
		const float eta;
		// BSDF Private Methods
		~BSDF();

	private:
		// BSDF Private Data
		const Normal3f ns, ng;
		const Vector3f ss, ts;
		int nBxDFs = 0;
		static constexpr int MaxBxDFs = 8;
		std::vector<std::shared_ptr<BxDF>> bxdfs;
		;
	};

	// BxDF Declarations
	class BxDF
	{
	public:
		// BxDF Interface
		virtual ~BxDF() {}
		BxDF(BxDFType type) : type(type) {}
		bool MatchesFlags(BxDFType t) const { return (type & t) == type; }
		virtual Spectrum f(const Vector3f &wo, const Vector3f &wi) const = 0;
		virtual Spectrum Sample_f(const Vector3f &wo, Vector3f *wi,
								  const Point2f &sample, float *pdf, BxDFType *sampledType = nullptr) const;
		virtual Spectrum rho(const Vector3f &wo, int nSamples, const Point2f *samples) const;
		virtual Spectrum rho(int nSamples, const Point2f *samples1, const Point2f *samples2) const;
		virtual float Pdf(const Vector3f &wo, const Vector3f &wi) const;

		// BxDF Public Data
		const BxDFType type;
	};

	class LambertianReflection : public BxDF
	{
	public:
		// LambertianReflection Public Methods
		LambertianReflection(const Spectrum &R)
			: BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(R) {}
		Spectrum f(const Vector3f &wo, const Vector3f &wi) const;
		Spectrum rho(const Vector3f &, int, const Point2f *) const { return R; }
		Spectrum rho(int, const Point2f *, const Point2f *) const { return R; }

	private:
		// LambertianReflection Private Data
		const Spectrum R;
	};

	// BSDF Inline Method Definitions
	inline int BSDF::NumComponents(BxDFType flags) const
	{
		int num = 0;
		for (int i = 0; i < nBxDFs; ++i)
			if (bxdfs[i]->MatchesFlags(flags))
				++num;
		return num;
	}

	class SpecularReflection : public BxDF
	{
	public:
		// SpecularReflection Public Methods
		SpecularReflection(const Spectrum &R, Fresnel *fresnel)
			: BxDF(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR)),
			  R(R),
			  fresnel(fresnel) {}
		~SpecularReflection() { fresnel->~Fresnel(); }
		virtual Spectrum f(const Vector3f &wo, const Vector3f &wi) const
		{
			return Spectrum(0.f);
		}
		virtual Spectrum Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample,
								  float *pdf, BxDFType *sampledType) const;
		float Pdf(const Vector3f &wo, const Vector3f &wi) const { return 0; }

	private:
		// SpecularReflection Private Data
		const Spectrum R;
		const Fresnel *fresnel;
	};

	class SpecularTransmission : public BxDF
	{
	public:
		// SpecularTransmission Public Methods
		SpecularTransmission(const Spectrum &T, float etaA, float etaB,
							 TransportMode mode)
			: BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR)),
			  T(T),
			  etaA(etaA),
			  etaB(etaB),
			  fresnel(etaA, etaB),
			  mode(mode) {}
		Spectrum f(const Vector3f &wo, const Vector3f &wi) const
		{
			return Spectrum(0.f);
		}
		Spectrum Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample,
						  float *pdf, BxDFType *sampledType) const;
		float Pdf(const Vector3f &wo, const Vector3f &wi) const { return 0; }

	private:
		// SpecularTransmission Private Data
		const Spectrum T;
		const float etaA, etaB;
		const FresnelDielectric fresnel;

		const TransportMode mode;
	};

	class FresnelSpecular : public BxDF
	{
	public:
		// FresnelSpecular Public Methods
		FresnelSpecular(const Spectrum &R, const Spectrum &T, float etaA, float etaB, TransportMode mode)
			: BxDF(BxDFType(BSDF_REFLECTION | BSDF_TRANSMISSION | BSDF_SPECULAR)),
			  R(R),
			  T(T),
			  etaA(etaA),
			  etaB(etaB),
			  mode(mode) {}
		Spectrum f(const Vector3f &wo, const Vector3f &wi) const
		{
			return Spectrum(0.f);
		}
		Spectrum Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &u,
						  float *pdf, BxDFType *sampledType) const;
		float Pdf(const Vector3f &wo, const Vector3f &wi) const { return 0; }

	private:
		// FresnelSpecular Private Data
		const Spectrum R, T;
		const float etaA, etaB;
		const TransportMode mode;
	};

	class OrenNayar : public BxDF
	{
	public:
		// OrenNayar Public Methods
		Spectrum f(const Vector3f &wo, const Vector3f &wi) const;
		OrenNayar(const Spectrum &R, float sigma)
			: BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(R)
		{
			sigma = Radians(sigma);
			float sigma2 = sigma * sigma;
			A = 1.f - (sigma2 / (2.f * (sigma2 + 0.33f)));
			B = 0.45f * sigma2 / (sigma2 + 0.09f);
		}

	private:
		// OrenNayar Private Data
		const Spectrum R;
		float A, B;
	};

	class MicrofacetReflection : public BxDF
	{
	public:
		// MicrofacetReflection Public Methods
		MicrofacetReflection(const Spectrum &R,
							 MicrofacetDistribution *distribution, Fresnel *fresnel)
			: BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)),
			  R(R),
			  distribution(distribution),
			  fresnel(fresnel) {}
		~MicrofacetReflection() {}
		Spectrum f(const Vector3f &wo, const Vector3f &wi) const;
		Spectrum Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &u,
						  float *pdf, BxDFType *sampledType) const;
		float Pdf(const Vector3f &wo, const Vector3f &wi) const;

	private:
		// MicrofacetReflection Private Data
		const Spectrum R;
		const std::shared_ptr<MicrofacetDistribution> distribution;
		const std::shared_ptr<Fresnel> fresnel;
	};

	class MicrofacetTransmission : public BxDF
	{
	public:
		// MicrofacetTransmission Public Methods
		MicrofacetTransmission(const Spectrum &T,
							   MicrofacetDistribution *distribution, float etaA,
							   float etaB, TransportMode mode)
			: BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_GLOSSY)),
			  T(T),
			  distribution(distribution),
			  etaA(etaA),
			  etaB(etaB),
			  fresnel(etaA, etaB),
			  mode(mode) {}
		~MicrofacetTransmission() {}
		Spectrum f(const Vector3f &wo, const Vector3f &wi) const;
		Spectrum Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &u,
						  float *pdf, BxDFType *sampledType) const;
		float Pdf(const Vector3f &wo, const Vector3f &wi) const;

	private:
		// MicrofacetTransmission Private Data
		const Spectrum T;
		const std::shared_ptr<MicrofacetDistribution> distribution;
		const float etaA, etaB;
		const FresnelDielectric fresnel;
		const TransportMode mode;
	};

	class FresnelBlend : public BxDF
	{
	public:
		// FresnelBlend Public Methods
		FresnelBlend(const Spectrum &Rd, const Spectrum &Rs,
					 MicrofacetDistribution *distrib);
		~FresnelBlend() {}
		Spectrum f(const Vector3f &wo, const Vector3f &wi) const;
		Spectrum SchlickFresnel(float cosTheta) const
		{
			auto pow5 = [](float v)
			{ return (v * v) * (v * v) * v; };
			return Rs + pow5(1 - cosTheta) * (Spectrum(1.) - Rs);
		}
		Spectrum Sample_f(const Vector3f &wi, Vector3f *sampled_f, const Point2f &u,
						  float *pdf, BxDFType *sampledType) const;
		float Pdf(const Vector3f &wo, const Vector3f &wi) const;

	private:
		// FresnelBlend Private Data
		const Spectrum Rd, Rs;
		std::shared_ptr<MicrofacetDistribution> distribution;
	};

}

#endif

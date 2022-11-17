#pragma once
#ifndef __Interaction_h__
#define __Interaction_h__

#include "Core/Geometry.h"
#include "Shape/Shape.h"
#include "Material/Material.h"
#include "Media/Medium.h"

namespace Feimos
{

	struct Interaction
	{
		// Interaction Public Methods
		Interaction() : time(0) {}
		Interaction(const Point3f &p, const Normal3f &n, const Vector3f &pError,
					const Vector3f &wo, float time, const MediumInterface &mediumInterface)
			: p(p),
			  time(time),
			  pError(pError),
			  wo(Normalize(wo)),
			  n(n),
			  mediumInterface(mediumInterface) {}
		Interaction(const Point3f &p, const Vector3f &wo, float time, const MediumInterface &mediumInterface)
			: p(p), time(time), wo(wo), mediumInterface(mediumInterface) {}
		Interaction(const Point3f &p, float time, const MediumInterface &mediumInterface)
			: p(p), time(time), mediumInterface(mediumInterface) {}

		bool IsSurfaceInteraction() const { return n != Normal3f(); }
		Ray SpawnRay(const Vector3f &d) const
		{
			Point3f o = OffsetRayOrigin(p, pError, n, d);
			return Ray(o, d, Infinity, time, GetMedium(d));
		}
		Ray SpawnRayTo(const Point3f &p2) const
		{
			Point3f origin = OffsetRayOrigin(p, pError, n, p2 - p);
			Vector3f d = p2 - p;
			return Ray(origin, d, 1 - ShadowEpsilon, time, GetMedium(d));
		}
		Ray SpawnRayTo(const Interaction &it) const
		{
			Point3f origin = OffsetRayOrigin(p, pError, n, it.p - p);
			Point3f target = OffsetRayOrigin(it.p, it.pError, it.n, origin - it.p);
			Vector3f d = target - origin;
			return Ray(origin, d, 1 - ShadowEpsilon, time, GetMedium(d));
		}
		bool IsMediumInteraction() const { return !IsSurfaceInteraction(); }
		const Medium *GetMedium(const Vector3f &w) const
		{
			return Dot(w, n) > 0 ? mediumInterface.outside : mediumInterface.inside;
		}
		const Medium *GetMedium() const
		{
			// CHECK_EQ(mediumInterface.inside, mediumInterface.outside);
			return mediumInterface.inside;
		}
		// Interaction Public Data
		Point3f p;
		float time;
		Vector3f pError;
		Vector3f wo;
		Normal3f n;
		MediumInterface mediumInterface;
	};

	class MediumInteraction : public Interaction
	{
	public:
		// MediumInteraction Public Methods
		MediumInteraction() : phase(nullptr) {}
		MediumInteraction(const Point3f &p, const Vector3f &wo, float time,
						  const Medium *medium, PhaseFunction *phase)
			: Interaction(p, wo, time, medium), phase(phase) {}
		bool IsValid() const { return phase != nullptr; }

		// MediumInteraction Public Data
		std::shared_ptr<PhaseFunction> phase = nullptr;
	};

	class SurfaceInteraction : public Interaction
	{
	public:
		// SurfaceInteraction Public Methods
		SurfaceInteraction() {}
		SurfaceInteraction(const Point3f &p, const Vector3f &pError,
						   const Point2f &uv, const Vector3f &wo,
						   const Vector3f &dpdu, const Vector3f &dpdv,
						   const Normal3f &dndu, const Normal3f &dndv, float time,
						   const Shape *sh,
						   int faceIndex = 0);
		~SurfaceInteraction();

		void ComputeDifferentials(const RayDifferential &ray) const;
		void ComputeScatteringFunctions(const Ray &ray, bool allowMultipleLobes = false, TransportMode mode = TransportMode::Radiance);
		void SetShadingGeometry(const Vector3f &dpdu, const Vector3f &dpdv, const Normal3f &dndu, const Normal3f &dndv, bool orientationIsAuthoritative);
		Spectrum Le(const Vector3f &w) const;

		const Primitive *primitive = nullptr;
		const Shape *shape = nullptr;

		std::shared_ptr<BSDF> bsdf = nullptr;

		Point2f uv;			 //纹理坐标
		Vector3f dpdu, dpdv; //三角形几何偏微分
		Normal3f dndu, dndv; //三角形几何法向量偏置
		struct
		{
			Normal3f n;
			Vector3f dpdu, dpdv;
			Normal3f dndu, dndv;
		} shading;
		mutable Vector3f dpdx, dpdy; //光线微分方向
		mutable float dudx = 0, dvdx = 0, dudy = 0, dvdy = 0;

		mutable bool testd = false;
	};

}

#endif

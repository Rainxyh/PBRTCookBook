#pragma once
#include "Core/Geometry.h"
#include "Material/Material.h"
#include "Media/Medium.h"
#include "Core/Transform.h"

namespace Feimos
{

	class Primitive
	{
	public:
		// Primitive Interface
		virtual ~Primitive();
		virtual Bounds3f WorldBound() const = 0;
		virtual bool Intersect(const Ray &r, SurfaceInteraction *) const = 0;
		virtual bool IntersectP(const Ray &r) const = 0;
		virtual const AreaLight *GetAreaLight() const = 0;
		virtual const Material *GetMaterial() const = 0;
		virtual void ComputeScatteringFunctions(SurfaceInteraction *isect,
												TransportMode mode,
												bool allowMultipleLobes) const = 0;
	};

	class GeometricPrimitive : public Primitive
	{
	public:
		// GeometricPrimitive Public Methods
		virtual Bounds3f WorldBound() const;
		virtual bool Intersect(const Ray &r, SurfaceInteraction *isect) const;
		virtual bool IntersectP(const Ray &r) const;
		GeometricPrimitive(const std::shared_ptr<Shape> &shape,
						   const std::shared_ptr<Material> &material,
						   const std::shared_ptr<AreaLight> &areaLight,
						   const MediumInterface &mediumInterface);

		const AreaLight *GetAreaLight() const;
		const Material *GetMaterial() const;

		virtual void ComputeScatteringFunctions(SurfaceInteraction *isect,
												TransportMode mode,
												bool allowMultipleLobes) const;

	private:
		// GeometricPrimitive Private Data
		std::shared_ptr<Material> material;
		std::shared_ptr<AreaLight> areaLight;
		std::shared_ptr<Shape> shape;
		MediumInterface mediumInterface;
	};

	// TransformedPrimitive Declarations
	class TransformedPrimitive : public Primitive
	{
	public:
		// TransformedPrimitive Public Methods
		TransformedPrimitive(std::shared_ptr<Primitive> &primitive,
							 const AnimatedTransform &PrimitiveToWorld);
		bool Intersect(const Ray &r, SurfaceInteraction *in) const;
		bool IntersectP(const Ray &r) const;
		const AreaLight *GetAreaLight() const { return nullptr; }
		const Material *GetMaterial() const { return nullptr; }
		void ComputeScatteringFunctions(SurfaceInteraction *isect, TransportMode mode,
										bool allowMultipleLobes) const
		{
			// ³ö´í
		}
		Bounds3f WorldBound() const
		{
			return PrimitiveToWorld.MotionBounds(primitive->WorldBound());
		}

	private:
		// TransformedPrimitive Private Data
		std::shared_ptr<Primitive> primitive;
		const AnimatedTransform PrimitiveToWorld;
	};

	class Aggregate : public Primitive
	{
	public:
		// Aggregate Public Methods
		virtual void ComputeScatteringFunctions(SurfaceInteraction *isect,
												TransportMode mode,
												bool allowMultipleLobes) const {}

		const AreaLight *GetAreaLight() const { return nullptr; }
		const Material *GetMaterial() const { return nullptr; }
	};

}

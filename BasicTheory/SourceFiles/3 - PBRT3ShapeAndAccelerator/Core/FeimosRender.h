#pragma once
#ifndef __FeimosRender_h__
#define __FeimosRender_h__

#include <memory>
#include <limits>
#include <math.h>
namespace Feimos
{

	static constexpr float Pi = 3.14159265358979323846;
	static constexpr float InvPi = 0.31830988618379067154;
	static constexpr float Inv2Pi = 0.15915494309189533577;
	static constexpr float Inv4Pi = 0.07957747154594766788;
	static constexpr float PiOver2 = 1.57079632679489661923;
	static constexpr float PiOver4 = 0.78539816339744830961;
	static constexpr float Sqrt2 = 1.41421356237309504880;
	inline constexpr float Radians(float deg) { return (Pi / 180) * deg; }

#define CHECK_NE(a, b)
#define DCHECK(a)

#define Infinity std::numeric_limits<float>::infinity()

	template <typename T>
	class Vector2;
	template <typename T>
	class Vector3;
	typedef Vector2<float> Vector2f;
	typedef Vector2<int> Vector2i;
	typedef Vector3<float> Vector3f;
	typedef Vector3<int> Vector3i;
	template <typename T>
	class Point3;
	template <typename T>
	class Point2;
	typedef Point2<float> Point2f;
	typedef Point2<int> Point2i;
	typedef Point3<float> Point3f;
	typedef Point3<int> Point3i;
	template <typename T>
	class Normal3;
	template <typename T>
	class Bounds2;
	template <typename T>
	class Bounds3;
	typedef Bounds2<float> Bounds2f;
	typedef Bounds2<int> Bounds2i;
	typedef Bounds3<float> Bounds3f;
	typedef Bounds3<int> Bounds3i;

	struct Matrix4x4;
	class Transform;
	class Ray;

	class Shape;
	struct Interaction;
	class SurfaceInteraction;
	class Primitive;
	class GeometricPrimitive;
	class Aggregate;

	template <typename T>
	inline bool isNaN(const T x)
	{
		return isnan(x);
	}
	template <>
	inline bool isNaN(const int x)
	{
		return false;
	}

}

#endif

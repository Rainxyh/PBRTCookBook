#pragma once
#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>

#include "Core\FeimosRender.h"

namespace Feimos {

	/*************************  向量、点和法向量   *******************************/
	// Vector Declarations
	template <typename T>
	class Vector2 {
	public:
		// Vector2 Public Methods
		Vector2() { x = y = 0; }
		Vector2(T xx, T yy) : x(xx), y(yy) { DCHECK(!HasNaNs()); }
		bool HasNaNs() const { return isNaN(x) || isNaN(y); }
		explicit Vector2(const Point2<T> &p);
		explicit Vector2(const Point3<T> &p);
#ifndef NDEBUG
		// The default versions of these are fine for release builds; for debug
		// we define them so that we can add the Assert checks.
		Vector2(const Vector2<T> &v) {
			DCHECK(!v.HasNaNs());
			x = v.x;
			y = v.y;
		}
		Vector2<T> &operator=(const Vector2<T> &v) {
			DCHECK(!v.HasNaNs());
			x = v.x;
			y = v.y;
			return *this;
		}
#endif  // !NDEBUG

		Vector2<T> operator+(const Vector2<T> &v) const {
			DCHECK(!v.HasNaNs());
			return Vector2(x + v.x, y + v.y);
		}

		Vector2<T> &operator+=(const Vector2<T> &v) {
			DCHECK(!v.HasNaNs());
			x += v.x;
			y += v.y;
			return *this;
		}
		Vector2<T> operator-(const Vector2<T> &v) const {
			DCHECK(!v.HasNaNs());
			return Vector2(x - v.x, y - v.y);
		}

		Vector2<T> &operator-=(const Vector2<T> &v) {
			DCHECK(!v.HasNaNs());
			x -= v.x;
			y -= v.y;
			return *this;
		}
		bool operator==(const Vector2<T> &v) const { return x == v.x && y == v.y; }
		bool operator!=(const Vector2<T> &v) const { return x != v.x || y != v.y; }
		template <typename U>
		Vector2<T> operator*(U f) const {
			return Vector2<T>(f * x, f * y);
		}

		template <typename U>
		Vector2<T> &operator*=(U f) {
			DCHECK(!isNaN(f));
			x *= f;
			y *= f;
			return *this;
		}
		template <typename U>
		Vector2<T> operator/(U f) const {
			CHECK_NE(f, 0);
			float inv = (float)1 / f;
			return Vector2<T>(x * inv, y * inv);
		}

		template <typename U>
		Vector2<T> &operator/=(U f) {
			CHECK_NE(f, 0);
			float inv = (float)1 / f;
			x *= inv;
			y *= inv;
			return *this;
		}
		Vector2<T> operator-() const { return Vector2<T>(-x, -y); }
		T operator[](int i) const {
			DCHECK(i >= 0 && i <= 1);
			if (i == 0) return x;
			return y;
		}

		T &operator[](int i) {
			DCHECK(i >= 0 && i <= 1);
			if (i == 0) return x;
			return y;
		}
		float LengthSquared() const { return x * x + y * y; }
		float Length() const { return std::sqrt(LengthSquared()); }

		// Vector2 Public Data
		T x, y;
	};

	template <typename T>
	class Vector3 {
	public:
		// Vector3 Public Methods
		T operator[](int i) const {
			DCHECK(i >= 0 && i <= 2);
			if (i == 0) return x;
			if (i == 1) return y;
			return z;
		}
		T &operator[](int i) {
			DCHECK(i >= 0 && i <= 2);
			if (i == 0) return x;
			if (i == 1) return y;
			return z;
		}
		Vector3() { x = y = z = 0; }
		Vector3(T x, T y, T z) : x(x), y(y), z(z) { DCHECK(!HasNaNs()); }
		bool HasNaNs() const { return isNaN(x) || isNaN(y) || isNaN(z); }
		explicit Vector3(const Point3<T> &p);
#ifndef NDEBUG
		// The default versions of these are fine for release builds; for debug
		// we define them so that we can add the Assert checks.
		Vector3(const Vector3<T> &v) {
			DCHECK(!v.HasNaNs());
			x = v.x;
			y = v.y;
			z = v.z;
		}

		Vector3<T> &operator=(const Vector3<T> &v) {
			DCHECK(!v.HasNaNs());
			x = v.x;
			y = v.y;
			z = v.z;
			return *this;
		}
#endif  // !NDEBUG
		Vector3<T> operator+(const Vector3<T> &v) const {
			DCHECK(!v.HasNaNs());
			return Vector3(x + v.x, y + v.y, z + v.z);
		}
		Vector3<T> &operator+=(const Vector3<T> &v) {
			DCHECK(!v.HasNaNs());
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}
		Vector3<T> operator-(const Vector3<T> &v) const {
			DCHECK(!v.HasNaNs());
			return Vector3(x - v.x, y - v.y, z - v.z);
		}
		Vector3<T> &operator-=(const Vector3<T> &v) {
			DCHECK(!v.HasNaNs());
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return *this;
		}
		bool operator==(const Vector3<T> &v) const {
			return x == v.x && y == v.y && z == v.z;
		}
		bool operator!=(const Vector3<T> &v) const {
			return x != v.x || y != v.y || z != v.z;
		}
		template <typename U>
		Vector3<T> operator*(U s) const {
			return Vector3<T>(s * x, s * y, s * z);
		}
		template <typename U>
		Vector3<T> &operator*=(U s) {
			DCHECK(!isNaN(s));
			x *= s;
			y *= s;
			z *= s;
			return *this;
		}
		template <typename U>
		Vector3<T> operator/(U f) const {
			CHECK_NE(f, 0);
			float inv = (float)1 / f;
			return Vector3<T>(x * inv, y * inv, z * inv);
		}

		template <typename U>
		Vector3<T> &operator/=(U f) {
			CHECK_NE(f, 0);
			float inv = (float)1 / f;
			x *= inv;
			y *= inv;
			z *= inv;
			return *this;
		}
		Vector3<T> operator-() const { return Vector3<T>(-x, -y, -z); }
		float LengthSquared() const { return x * x + y * y + z * z; }
		float Length() const { return std::sqrt(LengthSquared()); }
		explicit Vector3(const Normal3<T> &n);

		// Vector3 Public Data
		T x, y, z;
	};

	// Point Declarations
	template <typename T>
	class Point2 {
	public:
		// Point2 Public Methods
		explicit Point2(const Point3<T> &p) : x(p.x), y(p.y) { DCHECK(!HasNaNs()); }
		Point2() { x = y = 0; }
		Point2(T xx, T yy) : x(xx), y(yy) { DCHECK(!HasNaNs()); }

		template <typename U>
		explicit Point2(const Point2<U> &p) {
			x = (T)p.x;
			y = (T)p.y;
			DCHECK(!HasNaNs());
		}

		template <typename U>
		explicit Point2(const Vector2<U> &p) {
			x = (T)p.x;
			y = (T)p.y;
			DCHECK(!HasNaNs());
		}

		template <typename U>
		explicit operator Vector2<U>() const {
			return Vector2<U>(x, y);
		}

#ifndef NDEBUG
		Point2(const Point2<T> &p) {
			DCHECK(!p.HasNaNs());
			x = p.x;
			y = p.y;
		}

		Point2<T> &operator=(const Point2<T> &p) {
			DCHECK(!p.HasNaNs());
			x = p.x;
			y = p.y;
			return *this;
		}
#endif  // !NDEBUG
		Point2<T> operator+(const Vector2<T> &v) const {
			DCHECK(!v.HasNaNs());
			return Point2<T>(x + v.x, y + v.y);
		}

		Point2<T> &operator+=(const Vector2<T> &v) {
			DCHECK(!v.HasNaNs());
			x += v.x;
			y += v.y;
			return *this;
		}
		Vector2<T> operator-(const Point2<T> &p) const {
			DCHECK(!p.HasNaNs());
			return Vector2<T>(x - p.x, y - p.y);
		}

		Point2<T> operator-(const Vector2<T> &v) const {
			DCHECK(!v.HasNaNs());
			return Point2<T>(x - v.x, y - v.y);
		}
		Point2<T> operator-() const { return Point2<T>(-x, -y); }
		Point2<T> &operator-=(const Vector2<T> &v) {
			DCHECK(!v.HasNaNs());
			x -= v.x;
			y -= v.y;
			return *this;
		}
		Point2<T> &operator+=(const Point2<T> &p) {
			DCHECK(!p.HasNaNs());
			x += p.x;
			y += p.y;
			return *this;
		}
		Point2<T> operator+(const Point2<T> &p) const {
			DCHECK(!p.HasNaNs());
			return Point2<T>(x + p.x, y + p.y);
		}
		template <typename U>
		Point2<T> operator*(U f) const {
			return Point2<T>(f * x, f * y);
		}
		template <typename U>
		Point2<T> &operator*=(U f) {
			x *= f;
			y *= f;
			return *this;
		}
		template <typename U>
		Point2<T> operator/(U f) const {
			CHECK_NE(f, 0);
			float inv = (float)1 / f;
			return Point2<T>(inv * x, inv * y);
		}
		template <typename U>
		Point2<T> &operator/=(U f) {
			CHECK_NE(f, 0);
			float inv = (float)1 / f;
			x *= inv;
			y *= inv;
			return *this;
		}
		T operator[](int i) const {
			DCHECK(i >= 0 && i <= 1);
			if (i == 0) return x;
			return y;
		}

		T &operator[](int i) {
			DCHECK(i >= 0 && i <= 1);
			if (i == 0) return x;
			return y;
		}
		bool operator==(const Point2<T> &p) const { return x == p.x && y == p.y; }
		bool operator!=(const Point2<T> &p) const { return x != p.x || y != p.y; }
		bool HasNaNs() const { return isNaN(x) || isNaN(y); }

		// Point2 Public Data
		T x, y;
	};

	template <typename T>
	class Point3 {
	public:
		// Point3 Public Methods
		Point3() { x = y = z = 0; }
		Point3(T x, T y, T z) : x(x), y(y), z(z) { DCHECK(!HasNaNs()); }
		template <typename U>
		explicit Point3(const Point3<U> &p)
			: x((T)p.x), y((T)p.y), z((T)p.z) {
			DCHECK(!HasNaNs());
		}
		template <typename U>
		explicit operator Vector3<U>() const {
			return Vector3<U>(x, y, z);
		}
#ifndef NDEBUG
		Point3(const Point3<T> &p) {
			DCHECK(!p.HasNaNs());
			x = p.x;
			y = p.y;
			z = p.z;
		}

		Point3<T> &operator=(const Point3<T> &p) {
			DCHECK(!p.HasNaNs());
			x = p.x;
			y = p.y;
			z = p.z;
			return *this;
		}
#endif  // !NDEBUG
		Point3<T> operator+(const Vector3<T> &v) const {
			DCHECK(!v.HasNaNs());
			return Point3<T>(x + v.x, y + v.y, z + v.z);
		}
		Point3<T> &operator+=(const Vector3<T> &v) {
			DCHECK(!v.HasNaNs());
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}
		Vector3<T> operator-(const Point3<T> &p) const {
			DCHECK(!p.HasNaNs());
			return Vector3<T>(x - p.x, y - p.y, z - p.z);
		}
		Point3<T> operator-(const Vector3<T> &v) const {
			DCHECK(!v.HasNaNs());
			return Point3<T>(x - v.x, y - v.y, z - v.z);
		}
		Point3<T> &operator-=(const Vector3<T> &v) {
			DCHECK(!v.HasNaNs());
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return *this;
		}
		Point3<T> &operator+=(const Point3<T> &p) {
			DCHECK(!p.HasNaNs());
			x += p.x;
			y += p.y;
			z += p.z;
			return *this;
		}
		Point3<T> operator+(const Point3<T> &p) const {
			DCHECK(!p.HasNaNs());
			return Point3<T>(x + p.x, y + p.y, z + p.z);
		}
		template <typename U>
		Point3<T> operator*(U f) const {
			return Point3<T>(f * x, f * y, f * z);
		}
		template <typename U>
		Point3<T> &operator*=(U f) {
			x *= f;
			y *= f;
			z *= f;
			return *this;
		}
		template <typename U>
		Point3<T> operator/(U f) const {
			CHECK_NE(f, 0);
			float inv = (float)1 / f;
			return Point3<T>(inv * x, inv * y, inv * z);
		}
		template <typename U>
		Point3<T> &operator/=(U f) {
			CHECK_NE(f, 0);
			float inv = (float)1 / f;
			x *= inv;
			y *= inv;
			z *= inv;
			return *this;
		}
		T operator[](int i) const {
			DCHECK(i >= 0 && i <= 2);
			if (i == 0) return x;
			if (i == 1) return y;
			return z;
		}

		T &operator[](int i) {
			DCHECK(i >= 0 && i <= 2);
			if (i == 0) return x;
			if (i == 1) return y;
			return z;
		}
		bool operator==(const Point3<T> &p) const {
			return x == p.x && y == p.y && z == p.z;
		}
		bool operator!=(const Point3<T> &p) const {
			return x != p.x || y != p.y || z != p.z;
		}
		bool HasNaNs() const { return isNaN(x) || isNaN(y) || isNaN(z); }
		Point3<T> operator-() const { return Point3<T>(-x, -y, -z); }

		// Point3 Public Data
		T x, y, z;
	};

	// Normal Declarations
	template <typename T>
	class Normal3 {
	public:
		// Normal3 Public Methods
		Normal3() { x = y = z = 0; }
		Normal3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) { DCHECK(!HasNaNs()); }
		Normal3<T> operator-() const { return Normal3(-x, -y, -z); }
		Normal3<T> operator+(const Normal3<T> &n) const {
			DCHECK(!n.HasNaNs());
			return Normal3<T>(x + n.x, y + n.y, z + n.z);
		}

		Normal3<T> &operator+=(const Normal3<T> &n) {
			DCHECK(!n.HasNaNs());
			x += n.x;
			y += n.y;
			z += n.z;
			return *this;
		}
		Normal3<T> operator-(const Normal3<T> &n) const {
			DCHECK(!n.HasNaNs());
			return Normal3<T>(x - n.x, y - n.y, z - n.z);
		}

		Normal3<T> &operator-=(const Normal3<T> &n) {
			DCHECK(!n.HasNaNs());
			x -= n.x;
			y -= n.y;
			z -= n.z;
			return *this;
		}
		bool HasNaNs() const { return isNaN(x) || isNaN(y) || isNaN(z); }
		template <typename U>
		Normal3<T> operator*(U f) const {
			return Normal3<T>(f * x, f * y, f * z);
		}

		template <typename U>
		Normal3<T> &operator*=(U f) {
			x *= f;
			y *= f;
			z *= f;
			return *this;
		}
		template <typename U>
		Normal3<T> operator/(U f) const {
			CHECK_NE(f, 0);
			float inv = (float)1 / f;
			return Normal3<T>(x * inv, y * inv, z * inv);
		}

		template <typename U>
		Normal3<T> &operator/=(U f) {
			CHECK_NE(f, 0);
			float inv = (float)1 / f;
			x *= inv;
			y *= inv;
			z *= inv;
			return *this;
		}
		float LengthSquared() const { return x * x + y * y + z * z; }
		float Length() const { return std::sqrt(LengthSquared()); }

#ifndef NDEBUG
		Normal3<T>(const Normal3<T> &n) {
			DCHECK(!n.HasNaNs());
			x = n.x;
			y = n.y;
			z = n.z;
		}

		Normal3<T> &operator=(const Normal3<T> &n) {
			DCHECK(!n.HasNaNs());
			x = n.x;
			y = n.y;
			z = n.z;
			return *this;
		}
#endif  // !NDEBUG
		explicit Normal3<T>(const Vector3<T> &v) : x(v.x), y(v.y), z(v.z) {
			DCHECK(!v.HasNaNs());
		}
		bool operator==(const Normal3<T> &n) const {
			return x == n.x && y == n.y && z == n.z;
		}
		bool operator!=(const Normal3<T> &n) const {
			return x != n.x || y != n.y || z != n.z;
		}

		T operator[](int i) const {
			DCHECK(i >= 0 && i <= 2);
			if (i == 0) return x;
			if (i == 1) return y;
			return z;
		}

		T &operator[](int i) {
			DCHECK(i >= 0 && i <= 2);
			if (i == 0) return x;
			if (i == 1) return y;
			return z;
		}

		// Normal3 Public Data
		T x, y, z;
	};

	typedef Normal3<float> Normal3f;

	// Geometry Inline Functions
	template <typename T>
	inline Vector3<T>::Vector3(const Point3<T> &p)
		: x(p.x), y(p.y), z(p.z) {
		DCHECK(!HasNaNs());
	}

	template <typename T>
	Vector3<T> operator*(const Vector3<T> &v, const Vector3<T> &w) {
		return Vector3<T>(v.x * w.x, v.y * w.y, v.z * w.z);
	}

	template <typename T, typename U>
	inline Vector3<T> operator*(U s, const Vector3<T> &v) {
		return v * s;
	}

	template <typename T>
	Vector3<T> Abs(const Vector3<T> &v) {
		return Vector3<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z));
	}

	template <typename T>
	inline T Dot(const Vector3<T> &v1, const Vector3<T> &v2) {
		DCHECK(!v1.HasNaNs() && !v2.HasNaNs());
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	template <typename T>
	inline T AbsDot(const Vector3<T> &v1, const Vector3<T> &v2) {
		DCHECK(!v1.HasNaNs() && !v2.HasNaNs());
		return std::abs(Dot(v1, v2));
	}

	template <typename T>
	inline Vector3<T> Cross(const Vector3<T> &v1, const Vector3<T> &v2) {
		DCHECK(!v1.HasNaNs() && !v2.HasNaNs());
		double v1x = v1.x, v1y = v1.y, v1z = v1.z;
		double v2x = v2.x, v2y = v2.y, v2z = v2.z;
		return Vector3<T>((v1y * v2z) - (v1z * v2y), (v1z * v2x) - (v1x * v2z),
			(v1x * v2y) - (v1y * v2x));
	}

	template <typename T>
	inline Vector3<T> Cross(const Vector3<T> &v1, const Normal3<T> &v2) {
		DCHECK(!v1.HasNaNs() && !v2.HasNaNs());
		double v1x = v1.x, v1y = v1.y, v1z = v1.z;
		double v2x = v2.x, v2y = v2.y, v2z = v2.z;
		return Vector3<T>((v1y * v2z) - (v1z * v2y), (v1z * v2x) - (v1x * v2z),
			(v1x * v2y) - (v1y * v2x));
	}

	template <typename T>
	inline Vector3<T> Cross(const Normal3<T> &v1, const Vector3<T> &v2) {
		DCHECK(!v1.HasNaNs() && !v2.HasNaNs());
		double v1x = v1.x, v1y = v1.y, v1z = v1.z;
		double v2x = v2.x, v2y = v2.y, v2z = v2.z;
		return Vector3<T>((v1y * v2z) - (v1z * v2y), (v1z * v2x) - (v1x * v2z),
			(v1x * v2y) - (v1y * v2x));
	}

	template <typename T>
	inline Vector3<T> Normalize(const Vector3<T> &v) {
		return v / v.Length();
	}
	template <typename T>
	T MinComponent(const Vector3<T> &v) {
		return std::min(v.x, std::min(v.y, v.z));
	}

	template <typename T>
	T MaxComponent(const Vector3<T> &v) {
		return std::max(v.x, std::max(v.y, v.z));
	}

	template <typename T>
	int MaxDimension(const Vector3<T> &v) {
		return (v.x > v.y) ? ((v.x > v.z) ? 0 : 2) : ((v.y > v.z) ? 1 : 2);
	}

	template <typename T>
	Vector3<T> Min(const Vector3<T> &p1, const Vector3<T> &p2) {
		return Vector3<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y),
			std::min(p1.z, p2.z));
	}

	template <typename T>
	Vector3<T> Max(const Vector3<T> &p1, const Vector3<T> &p2) {
		return Vector3<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y),
			std::max(p1.z, p2.z));
	}

	template <typename T>
	Vector3<T> Permute(const Vector3<T> &v, int x, int y, int z) {
		return Vector3<T>(v[x], v[y], v[z]);
	}

	template <typename T>
	Vector2<T>::Vector2(const Point2<T> &p)
		: x(p.x), y(p.y) {
		DCHECK(!HasNaNs());
	}

	template <typename T>
	Vector2<T>::Vector2(const Point3<T> &p)
		: x(p.x), y(p.y) {
		DCHECK(!HasNaNs());
	}

	template <typename T, typename U>
	inline Vector2<T> operator*(U f, const Vector2<T> &v) {
		return v * f;
	}
	template <typename T>
	inline float Dot(const Vector2<T> &v1, const Vector2<T> &v2) {
		DCHECK(!v1.HasNaNs() && !v2.HasNaNs());
		return v1.x * v2.x + v1.y * v2.y;
	}

	template <typename T>
	inline float AbsDot(const Vector2<T> &v1, const Vector2<T> &v2) {
		DCHECK(!v1.HasNaNs() && !v2.HasNaNs());
		return std::abs(Dot(v1, v2));
	}

	template <typename T>
	inline Vector2<T> Normalize(const Vector2<T> &v) {
		return v / v.Length();
	}
	template <typename T>
	Vector2<T> Abs(const Vector2<T> &v) {
		return Vector2<T>(std::abs(v.x), std::abs(v.y));
	}

	template <typename T>
	inline float Distance(const Point3<T> &p1, const Point3<T> &p2) {
		return (p1 - p2).Length();
	}

	template <typename T>
	inline float DistanceSquared(const Point3<T> &p1, const Point3<T> &p2) {
		return (p1 - p2).LengthSquared();
	}

	template <typename T, typename U>
	inline Point3<T> operator*(U f, const Point3<T> &p) {
		DCHECK(!p.HasNaNs());
		return p * f;
	}

	template <typename T>
	Point3<T> Lerp(float t, const Point3<T> &p0, const Point3<T> &p1) {
		return (1 - t) * p0 + t * p1;
	}

	template <typename T>
	Point3<T> Min(const Point3<T> &p1, const Point3<T> &p2) {
		return Point3<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y),
			std::min(p1.z, p2.z));
	}

	template <typename T>
	Point3<T> Max(const Point3<T> &p1, const Point3<T> &p2) {
		return Point3<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y),
			std::max(p1.z, p2.z));
	}

	template <typename T>
	Point3<T> Floor(const Point3<T> &p) {
		return Point3<T>(std::floor(p.x), std::floor(p.y), std::floor(p.z));
	}

	template <typename T>
	Point3<T> Ceil(const Point3<T> &p) {
		return Point3<T>(std::ceil(p.x), std::ceil(p.y), std::ceil(p.z));
	}

	template <typename T>
	Point3<T> Abs(const Point3<T> &p) {
		return Point3<T>(std::abs(p.x), std::abs(p.y), std::abs(p.z));
	}

	template <typename T>
	inline float Distance(const Point2<T> &p1, const Point2<T> &p2) {
		return (p1 - p2).Length();
	}

	template <typename T>
	inline float DistanceSquared(const Point2<T> &p1, const Point2<T> &p2) {
		return (p1 - p2).LengthSquared();
	}

	template <typename T, typename U>
	inline Point2<T> operator*(U f, const Point2<T> &p) {
		DCHECK(!p.HasNaNs());
		return p * f;
	}

	template <typename T>
	Point2<T> Floor(const Point2<T> &p) {
		return Point2<T>(std::floor(p.x), std::floor(p.y));
	}

	template <typename T>
	Point2<T> Ceil(const Point2<T> &p) {
		return Point2<T>(std::ceil(p.x), std::ceil(p.y));
	}

	template <typename T>
	Point2<T> Lerp(float t, const Point2<T> &v0, const Point2<T> &v1) {
		return (1 - t) * v0 + t * v1;
	}

	template <typename T>
	Point2<T> Min(const Point2<T> &pa, const Point2<T> &pb) {
		return Point2<T>(std::min(pa.x, pb.x), std::min(pa.y, pb.y));
	}

	template <typename T>
	Point2<T> Max(const Point2<T> &pa, const Point2<T> &pb) {
		return Point2<T>(std::max(pa.x, pb.x), std::max(pa.y, pb.y));
	}

	template <typename T>
	Point3<T> Permute(const Point3<T> &p, int x, int y, int z) {
		return Point3<T>(p[x], p[y], p[z]);
	}

	template <typename T, typename U>
	inline Normal3<T> operator*(U f, const Normal3<T> &n) {
		return Normal3<T>(f * n.x, f * n.y, f * n.z);
	}

	template <typename T>
	inline Normal3<T> Normalize(const Normal3<T> &n) {
		return n / n.Length();
	}

	template <typename T>
	inline Vector3<T>::Vector3(const Normal3<T> &n)
		: x(n.x), y(n.y), z(n.z) {
		DCHECK(!n.HasNaNs());
	}

	template <typename T>
	inline T Dot(const Normal3<T> &n1, const Vector3<T> &v2) {
		DCHECK(!n1.HasNaNs() && !v2.HasNaNs());
		return n1.x * v2.x + n1.y * v2.y + n1.z * v2.z;
	}

	template <typename T>
	inline T Dot(const Vector3<T> &v1, const Normal3<T> &n2) {
		DCHECK(!v1.HasNaNs() && !n2.HasNaNs());
		return v1.x * n2.x + v1.y * n2.y + v1.z * n2.z;
	}

	template <typename T>
	inline T Dot(const Normal3<T> &n1, const Normal3<T> &n2) {
		DCHECK(!n1.HasNaNs() && !n2.HasNaNs());
		return n1.x * n2.x + n1.y * n2.y + n1.z * n2.z;
	}

	template <typename T>
	inline T AbsDot(const Normal3<T> &n1, const Vector3<T> &v2) {
		DCHECK(!n1.HasNaNs() && !v2.HasNaNs());
		return std::abs(n1.x * v2.x + n1.y * v2.y + n1.z * v2.z);
	}

	template <typename T>
	inline T AbsDot(const Vector3<T> &v1, const Normal3<T> &n2) {
		DCHECK(!v1.HasNaNs() && !n2.HasNaNs());
		return std::abs(v1.x * n2.x + v1.y * n2.y + v1.z * n2.z);
	}

	template <typename T>
	inline T AbsDot(const Normal3<T> &n1, const Normal3<T> &n2) {
		DCHECK(!n1.HasNaNs() && !n2.HasNaNs());
		return std::abs(n1.x * n2.x + n1.y * n2.y + n1.z * n2.z);
	}

	template <typename T>
	inline Normal3<T> Faceforward(const Normal3<T> &n, const Vector3<T> &v) {
		return (Dot(n, v) < 0.f) ? -n : n;
	}

	template <typename T>
	inline Normal3<T> Faceforward(const Normal3<T> &n, const Normal3<T> &n2) {
		return (Dot(n, n2) < 0.f) ? -n : n;
	}

	template <typename T>
	inline Vector3<T> Faceforward(const Vector3<T> &v, const Vector3<T> &v2) {
		return (Dot(v, v2) < 0.f) ? -v : v;
	}

	template <typename T>
	inline Vector3<T> Faceforward(const Vector3<T> &v, const Normal3<T> &n2) {
		return (Dot(v, n2) < 0.f) ? -v : v;
	}

	template <typename T>
	Normal3<T> Abs(const Normal3<T> &v) {
		return Normal3<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z));
	}


	// Bounds Declarations
	template <typename T>
	class Bounds2 {
	public:
		// Bounds2 Public Methods
		Bounds2() {
			T minNum = std::numeric_limits<T>::lowest();
			T maxNum = std::numeric_limits<T>::max();
			pMin = Point2<T>(maxNum, maxNum);
			pMax = Point2<T>(minNum, minNum);
		}
		explicit Bounds2(const Point2<T> &p) : pMin(p), pMax(p) {}
		Bounds2(const Point2<T> &p1, const Point2<T> &p2) {
			pMin = Point2<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y));
			pMax = Point2<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y));
		}
		template <typename U>
		explicit operator Bounds2<U>() const {
			return Bounds2<U>((Point2<U>)pMin, (Point2<U>)pMax);
		}

		Vector2<T> Diagonal() const { return pMax - pMin; }
		T Area() const {
			Vector2<T> d = pMax - pMin;
			return (d.x * d.y);
		}
		int MaximumExtent() const {
			Vector2<T> diag = Diagonal();
			if (diag.x > diag.y)
				return 0;
			else
				return 1;
		}
		inline const Point2<T> &operator[](int i) const {
			DCHECK(i == 0 || i == 1);
			return (i == 0) ? pMin : pMax;
		}
		inline Point2<T> &operator[](int i) {
			DCHECK(i == 0 || i == 1);
			return (i == 0) ? pMin : pMax;
		}
		bool operator==(const Bounds2<T> &b) const {
			return b.pMin == pMin && b.pMax == pMax;
		}
		bool operator!=(const Bounds2<T> &b) const {
			return b.pMin != pMin || b.pMax != pMax;
		}
		Point2<T> Lerp(const Point2f &t) const {
			return Point2<T>(Lerp(t.x, pMin.x, pMax.x),
				Lerp(t.y, pMin.y, pMax.y));
		}
		Vector2<T> Offset(const Point2<T> &p) const {
			Vector2<T> o = p - pMin;
			if (pMax.x > pMin.x) o.x /= pMax.x - pMin.x;
			if (pMax.y > pMin.y) o.y /= pMax.y - pMin.y;
			return o;
		}
		void BoundingSphere(Point2<T> *c, float *rad) const {
			*c = (pMin + pMax) / 2;
			*rad = Inside(*c, *this) ? Distance(*c, pMax) : 0;
		}

		// Bounds2 Public Data
		Point2<T> pMin, pMax;
	};

	template <typename T>
	Bounds2<T> Union(const Bounds2<T> &b, const Point2<T> &p) {
		Bounds2<T> ret;
		ret.pMin = Min(b.pMin, p);
		ret.pMax = Max(b.pMax, p);
		return ret;
	}

	template <typename T>
	Bounds2<T> Union(const Bounds2<T> &b, const Bounds2<T> &b2) {
		Bounds2<T> ret;
		ret.pMin = Min(b.pMin, b2.pMin);
		ret.pMax = Max(b.pMax, b2.pMax);
		return ret;
	}

	template <typename T>
	Bounds2<T> Intersect(const Bounds2<T> &b1, const Bounds2<T> &b2) {
		// Important: assign to pMin/pMax directly and don't run the Bounds2()
		// constructor, since it takes min/max of the points passed to it.  In
		// turn, that breaks returning an invalid bound for the case where we
		// intersect non-overlapping bounds (as we'd like to happen).
		Bounds2<T> ret;
		ret.pMin = Max(b1.pMin, b2.pMin);
		ret.pMax = Min(b1.pMax, b2.pMax);
		return ret;
	}

	template <typename T>
	bool Overlaps(const Bounds2<T> &ba, const Bounds2<T> &bb) {
		bool x = (ba.pMax.x >= bb.pMin.x) && (ba.pMin.x <= bb.pMax.x);
		bool y = (ba.pMax.y >= bb.pMin.y) && (ba.pMin.y <= bb.pMax.y);
		return (x && y);
	}

	template <typename T>
	bool Inside(const Point2<T> &pt, const Bounds2<T> &b) {
		return (pt.x >= b.pMin.x && pt.x <= b.pMax.x && pt.y >= b.pMin.y &&
			pt.y <= b.pMax.y);
	}

	template <typename T>
	bool InsideExclusive(const Point2<T> &pt, const Bounds2<T> &b) {
		return (pt.x >= b.pMin.x && pt.x < b.pMax.x && pt.y >= b.pMin.y &&
			pt.y < b.pMax.y);
	}

	template <typename T, typename U>
	Bounds2<T> Expand(const Bounds2<T> &b, U delta) {
		return Bounds2<T>(b.pMin - Vector2<T>(delta, delta),
			b.pMax + Vector2<T>(delta, delta));
	}

	template <typename T>
	class Bounds3 {
	public:
		// Bounds3 Public Methods
		Bounds3() {
			T minNum = std::numeric_limits<T>::lowest();
			T maxNum = std::numeric_limits<T>::max();
			pMin = Point3<T>(maxNum, maxNum, maxNum);
			pMax = Point3<T>(minNum, minNum, minNum);
		}
		explicit Bounds3(const Point3<T> &p) : pMin(p), pMax(p) {}
		Bounds3(const Point3<T> &p1, const Point3<T> &p2)
			: pMin(std::min(p1.x, p2.x), std::min(p1.y, p2.y),
				std::min(p1.z, p2.z)),
			pMax(std::max(p1.x, p2.x), std::max(p1.y, p2.y),
				std::max(p1.z, p2.z)) {}
		const Point3<T> &operator[](int i) const;
		Point3<T> &operator[](int i);
		bool operator==(const Bounds3<T> &b) const {
			return b.pMin == pMin && b.pMax == pMax;
		}
		bool operator!=(const Bounds3<T> &b) const {
			return b.pMin != pMin || b.pMax != pMax;
		}
		Point3<T> Corner(int corner) const {
			DCHECK(corner >= 0 && corner < 8);
			return Point3<T>((*this)[(corner & 1)].x,
				(*this)[(corner & 2) ? 1 : 0].y,
				(*this)[(corner & 4) ? 1 : 0].z);
		}
		Vector3<T> Diagonal() const { return pMax - pMin; }
		T SurfaceArea() const {
			Vector3<T> d = Diagonal();
			return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
		}
		T Volume() const {
			Vector3<T> d = Diagonal();
			return d.x * d.y * d.z;
		}
		int MaximumExtent() const {
			Vector3<T> d = Diagonal();
			if (d.x > d.y && d.x > d.z)
				return 0;
			else if (d.y > d.z)
				return 1;
			else
				return 2;
		}
		Point3<T> Lerp(const Point3f &t) const {
			return Point3<T>(Lerp(t.x, pMin.x, pMax.x),
				Lerp(t.y, pMin.y, pMax.y),
				Lerp(t.z, pMin.z, pMax.z));
		}
		Vector3<T> Offset(const Point3<T> &p) const {
			Vector3<T> o = p - pMin;
			if (pMax.x > pMin.x) o.x /= pMax.x - pMin.x;
			if (pMax.y > pMin.y) o.y /= pMax.y - pMin.y;
			if (pMax.z > pMin.z) o.z /= pMax.z - pMin.z;
			return o;
		}
		void BoundingSphere(Point3<T> *center, float *radius) const {
			*center = (pMin + pMax) / 2;
			*radius = Inside(*center, *this) ? Distance(*center, pMax) : 0;
		}
		template <typename U>
		explicit operator Bounds3<U>() const {
			return Bounds3<U>((Point3<U>)pMin, (Point3<U>)pMax);
		}
		inline bool IntersectP(const Ray &ray, float *hitt0 = nullptr,
			float *hitt1 = nullptr) const;
		inline bool IntersectP(const Ray &ray, const Vector3f &invDir,
			const int dirIsNeg[3]) const;
		// Bounds3 Public Data
		Point3<T> pMin, pMax;
	};

	template <typename T>
	inline const Point3<T> &Bounds3<T>::operator[](int i) const {
		DCHECK(i == 0 || i == 1);
		return (i == 0) ? pMin : pMax;
	}

	template <typename T>
	inline Point3<T> &Bounds3<T>::operator[](int i) {
		DCHECK(i == 0 || i == 1);
		return (i == 0) ? pMin : pMax;
	}

	template <typename T>
	Bounds3<T> Union(const Bounds3<T> &b, const Point3<T> &p) {
		Bounds3<T> ret;
		ret.pMin = Min(b.pMin, p);
		ret.pMax = Max(b.pMax, p);
		return ret;
	}

	template <typename T>
	Bounds3<T> Union(const Bounds3<T> &b1, const Bounds3<T> &b2) {
		Bounds3<T> ret;
		ret.pMin = Min(b1.pMin, b2.pMin);
		ret.pMax = Max(b1.pMax, b2.pMax);
		return ret;
	}

	template <typename T>
	Bounds3<T> Intersect(const Bounds3<T> &b1, const Bounds3<T> &b2) {
		// Important: assign to pMin/pMax directly and don't run the Bounds2()
		// constructor, since it takes min/max of the points passed to it.  In
		// turn, that breaks returning an invalid bound for the case where we
		// intersect non-overlapping bounds (as we'd like to happen).
		Bounds3<T> ret;
		ret.pMin = Max(b1.pMin, b2.pMin);
		ret.pMax = Min(b1.pMax, b2.pMax);
		return ret;
	}

	template <typename T>
	bool Overlaps(const Bounds3<T> &b1, const Bounds3<T> &b2) {
		bool x = (b1.pMax.x >= b2.pMin.x) && (b1.pMin.x <= b2.pMax.x);
		bool y = (b1.pMax.y >= b2.pMin.y) && (b1.pMin.y <= b2.pMax.y);
		bool z = (b1.pMax.z >= b2.pMin.z) && (b1.pMin.z <= b2.pMax.z);
		return (x && y && z);
	}

	template <typename T>
	bool Inside(const Point3<T> &p, const Bounds3<T> &b) {
		return (p.x >= b.pMin.x && p.x <= b.pMax.x && p.y >= b.pMin.y &&
			p.y <= b.pMax.y && p.z >= b.pMin.z && p.z <= b.pMax.z);
	}

	template <typename T>
	bool InsideExclusive(const Point3<T> &p, const Bounds3<T> &b) {
		return (p.x >= b.pMin.x && p.x < b.pMax.x && p.y >= b.pMin.y &&
			p.y < b.pMax.y && p.z >= b.pMin.z && p.z < b.pMax.z);
	}

	template <typename T, typename U>
	inline Bounds3<T> Expand(const Bounds3<T> &b, U delta) {
		return Bounds3<T>(b.pMin - Vector3<T>(delta, delta, delta),
			b.pMax + Vector3<T>(delta, delta, delta));
	}

	// Minimum squared distance from point to box; returns zero if point is
	// inside.
	template <typename T, typename U>
	inline float DistanceSquared(const Point3<T> &p, const Bounds3<U> &b) {
		float dx = std::max({ float(0), b.pMin.x - p.x, p.x - b.pMax.x });
		float dy = std::max({ float(0), b.pMin.y - p.y, p.y - b.pMax.y });
		float dz = std::max({ float(0), b.pMin.z - p.z, p.z - b.pMax.z });
		return dx * dx + dy * dy + dz * dz;
	}

	template <typename T, typename U>
	inline float Distance(const Point3<T> &p, const Bounds3<U> &b) {
		return std::sqrt(DistanceSquared(p, b));
	}

	class Ray {
	public:
		// Ray Public Methods
		Ray() : tMax(Infinity), time(0.f) {}
		Ray(const Point3f &o, const Vector3f &d, float tMax = Infinity,
			float time = 0.f)
			: o(o), d(d), tMax(tMax), time(time) {}
		Point3f operator()(float t) const { return o + d * t; }
		bool HasNaNs() const { return (o.HasNaNs() || d.HasNaNs() || isNaN(tMax)); }
		// Ray Public Data
		Point3f o;
		Vector3f d;
		mutable float tMax;
		float time;
	};

	template <typename T>
	inline bool Bounds3<T>::IntersectP(const Ray &ray, float *hitt0,
		float *hitt1) const {
		float t0 = 0, t1 = ray.tMax;
		for (int i = 0; i < 3; ++i) {
			// Update interval for _i_th bounding box slab
			float invRayDir = 1 / ray.d[i];
			float tNear = (pMin[i] - ray.o[i]) * invRayDir;
			float tFar = (pMax[i] - ray.o[i]) * invRayDir;

			// Update parametric interval from slab intersection $t$ values
			if (tNear > tFar) std::swap(tNear, tFar);

			// Update _tFar_ to ensure robust ray--bounds intersection
#ifdef Feimos_Ray_Bound_ErrorBound
			tFar *= 1 + 2 * gamma(3);
#endif
			t0 = tNear > t0 ? tNear : t0;
			t1 = tFar < t1 ? tFar : t1;
			if (t0 > t1) return false;
		}
		if (hitt0) *hitt0 = t0;
		if (hitt1) *hitt1 = t1;
		return true;
	}

	template <typename T>
	inline bool Bounds3<T>::IntersectP(const Ray &ray, const Vector3f &invDir,
		const int dirIsNeg[3]) const {
		const Bounds3f &bounds = *this;
		// Check for ray intersection against $x$ and $y$ slabs
		float tMin = (bounds[dirIsNeg[0]].x - ray.o.x) * invDir.x;
		float tMax = (bounds[1 - dirIsNeg[0]].x - ray.o.x) * invDir.x;
		float tyMin = (bounds[dirIsNeg[1]].y - ray.o.y) * invDir.y;
		float tyMax = (bounds[1 - dirIsNeg[1]].y - ray.o.y) * invDir.y;

		// Update _tMax_ and _tyMax_ to ensure robust bounds intersection
#ifdef Feimos_Ray_Bound_ErrorBound
		tMax *= 1 + 2 * gamma(3);
		tyMax *= 1 + 2 * gamma(3);
#endif
		if (tMin > tyMax || tyMin > tMax) return false;
		if (tyMin > tMin) tMin = tyMin;
		if (tyMax < tMax) tMax = tyMax;

		// Check for ray intersection against $z$ slab
		float tzMin = (bounds[dirIsNeg[2]].z - ray.o.z) * invDir.z;
		float tzMax = (bounds[1 - dirIsNeg[2]].z - ray.o.z) * invDir.z;

		// Update _tzMax_ to ensure robust bounds intersection
#ifdef Feimos_Ray_Bound_ErrorBound
		tzMax *= 1 + 2 * gamma(3);
#endif
		if (tMin > tzMax || tzMin > tMax) return false;
		if (tzMin > tMin) tMin = tzMin;
		if (tzMax < tMax) tMax = tzMax;
		return (tMin < ray.tMax) && (tMax > 0);
	}

	inline Point3f OffsetRayOrigin(const Point3f &p, const Vector3f &pError,
		const Normal3f &n, const Vector3f &w) {
		float d = Dot(Abs(n), pError);
		Vector3f offset = d * Vector3f(n);
		if (Dot(w, n) < 0) offset = -offset;
		Point3f po = p + offset;
		// Round offset point _po_ away from _p_
		for (int i = 0; i < 3; ++i) {
			if (offset[i] > 0)
				po[i] = NextFloatUp(po[i]);
			else if (offset[i] < 0)
				po[i] = NextFloatDown(po[i]);
		}
		return po;
	}

	inline Vector3f reflect(const Vector3f& in, const Vector3f& normal)
	{
		// assumes unit length normal
		return in - normal * (2 * Dot(in, normal));
	}
	inline bool refract(const Vector3f&v, const  Vector3f &n, float ni_over_nt, Vector3f& refracted) {
		Vector3f uv = Normalize(v);
		float dt = Dot(uv, n);
		float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1 - dt*dt);
		if (discriminant > 0) {
			refracted = (uv - n*dt)*ni_over_nt - n*sqrt(discriminant);
			return true;
		}
		else
			return false;
	}



}



#endif

















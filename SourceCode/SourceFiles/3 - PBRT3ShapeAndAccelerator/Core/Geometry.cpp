#include "Geometry.hpp"
namespace Feimos
{
    template <typename T>
    Vector2<T> Vector2<T>::operator+(const Vector2<T> &v) const
    {
        DCHECK(!v.HasNaNs());
        return Vector2(x + v.x, y + v.y);
    }

    template <typename T>
    Vector2<T> &Vector2<T>::operator+=(const Vector2<T> &v)
    {
        DCHECK(!v.HasNaNs());
        x += v.x;
        y += v.y;
        return *this;
    }

    template <typename T>
    Vector2<T> Vector2<T>::operator-(const Vector2<T> &v) const
    {
        DCHECK(!v.HasNaNs());
        return Vector2(x - v.x, y - v.y);
    }

    template <typename T>
    Vector2<T> &Vector2<T>::operator-=(const Vector2<T> &v)
    {
        DCHECK(!v.HasNaNs());
        x -= v.x;
        y -= v.y;
        return *this;
    }

    template <typename T>
    bool Vector2<T>::operator==(const Vector2<T> &v) const { return x == v.x && y == v.y; }

    template <typename T>
    bool Vector2<T>::operator!=(const Vector2<T> &v) const { return x != v.x || y != v.y; }

    template <typename T>
    template <typename U>
    Vector2<T> Vector2<T>::operator*(U f) const
    {
        return Vector2<T>(f * x, f * y);
    }

    template <typename T>
    template <typename U>
    Vector2<T> &Vector2<T>::operator*=(U f)
    {
        DCHECK(!isNaN(f));
        x *= f;
        y *= f;
        return *this;
    }

    template <typename T>
    template <typename U>
    Vector2<T> Vector2<T>::operator/(U f) const
    {
        CHECK_NE(f, 0);
        float inv = (float)1 / f;
        return Vector2<T>(x * inv, y * inv);
    }

    template <typename T>
    template <typename U>
    Vector2<T> &Vector2<T>::operator/=(U f)
    {
        CHECK_NE(f, 0);
        float inv = (float)1 / f;
        x *= inv;
        y *= inv;
        return *this;
    }

    template <typename T>
    Vector2<T> Vector2<T>::operator-() const { return Vector2<T>(-x, -y); }

    template <typename T>
    T Vector2<T>::operator[](int i) const
    {
        DCHECK(i >= 0 && i <= 1);
        if (i == 0)
            return x;
        return y;
    }

    template <typename T>
    T &Vector2<T>::operator[](int i)
    {
        DCHECK(i >= 0 && i <= 1);
        if (i == 0)
            return x;
        return y;
    }

    template <typename T>
    float Vector2<T>::LengthSquared() const { return x * x + y * y; }

    template <typename T>
    float Vector2<T>::Length() const { return std::sqrt(LengthSquared()); }
}

///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Maths/Vec4.hpp"
#include <math.h>

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec4<T>::Vec4(void)
    : x(static_cast<T>(0))
    , y(static_cast<T>(0))
    , z(static_cast<T>(0))
    , w(static_cast<T>(0))
{}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec4<T>::Vec4(const T& all)
    : x(all)
    , y(all)
    , z(all)
    , w(all)
{}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec4<T>::Vec4(const T& x, const T& y, const T& z, const T& w)
    : x(x)
    , y(y)
    , z(z)
    , w(w)
{}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec4<T> Vec4<T>::Cross(const Vec4<T>& other) const
{
    return (Vec4<T>(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x,
        T(0)
    ));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
T Vec4<T>::Dot(const Vec4<T>& other) const
{
    return (x * other.x + y * other.y + z * other.z + w * other.w);
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
T Vec4<T>::LengthSquared(void) const
{
    return (x * x + y * y + z * z + w * w);
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec4<T> Vec4<T>::Normalize(void) const
{
    T lengthSquared = LengthSquared();

    if (lengthSquared < static_cast<T>(1e-10))
    {
        return (Vec4<T>(T(0)));
    }

    T invLength = static_cast<T>(1) / std::sqrt(lengthSquared);
    return ((*this) * invLength);
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec4<T> operator+(const Vec4<T>& lhs, const Vec4<T>& rhs)
{
    return (Vec4<T>(
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z,
        lhs.w + rhs.w
    ));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec4<T> operator-(const Vec4<T>& lhs, const Vec4<T>& rhs)
{
    return (Vec4<T>(
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z,
        lhs.w - rhs.w
    ));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec4<T> operator*(const Vec4<T>& lhs, const Vec4<T>& rhs)
{
    return (Vec4<T>(
        lhs.x * rhs.x,
        lhs.y * rhs.y,
        lhs.z * rhs.z,
        lhs.w * rhs.w
    ));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec4<T> operator*(const Vec4<T>& lhs, const T& rhs)
{
    return (Vec4<T>(
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs,
        lhs.w * rhs
    ));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec4<T> operator*(const T& lhs, const Vec4<T>& rhs)
{
    return (Vec4<T>(
        lhs * rhs.x,
        lhs * rhs.y,
        lhs * rhs.z,
        lhs * rhs.w
    ));
}

} // namespace Ray

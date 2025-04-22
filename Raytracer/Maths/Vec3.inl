///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Maths/Vec3.hpp"
#include <cmath>

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T>::Vec3(void)
    : x(static_cast<T>(0))
    , y(static_cast<T>(0))
    , z(static_cast<T>(0))
{}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T>::Vec3(const T& all)
    : x(all)
    , y(all)
    , z(all)
{}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T>::Vec3(const T& x, const T& y, const T& z)
    : x(x)
    , y(y)
    , z(z)
{}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T>::Vec3(const Vec4<T>& other)
    : x(other.x)
    , y(other.y)
    , z(other.z)
{}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
T& Vec3<T>::operator[](Uint64 index)
{
    RAY_ASSERT(index < 3, "Index Out of Range");
    return (data[index]);
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
const T& Vec3<T>::operator[](Uint64 index) const
{
    RAY_ASSERT(index < 3, "Index Out of Range");
    return (data[index]);
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T> Vec3<T>::Min(const Vec3<T>& a, const Vec3<T>& b)
{
    return (Vec3<T>(
        std::min(a.x, b.x),
        std::min(a.y, b.y),
        std::min(a.z, b.z)
    ));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T> Vec3<T>::Min(const Vec3<T>& a, const Vec3<T>& b, const Vec3<T>& c)
{
    return (Min(Min(a, b), c));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T> Vec3<T>::Max(const Vec3<T>& a, const Vec3<T>& b)
{
    return (Vec3<T>(
        std::max(a.x, b.x),
        std::max(a.y, b.y),
        std::max(a.z, b.z)
    ));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T> Vec3<T>::Max(const Vec3<T>& a, const Vec3<T>& b, const Vec3<T>& c)
{
    return (Max(Max(a, b), c));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T> Vec3<T>::Cross(const Vec3<T>& a, const Vec3<T>& b)
{
    return (Vec3<T>(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    ));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
T Vec3<T>::Length(const Vec3<T>& vec)
{
    return (std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T> Vec3<T>::Normalize(const Vec3<T>& vec)
{
    float length = Vec3<T>::Length(vec);
    return (Vec3<T>(
        vec.x / length,
        vec.y / length,
        vec.z / length
    ));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
T Vec3<T>::Distance(const Vec3<T>& a, const Vec3<T>& b)
{
    return (Vec3<T>::Length(a - b));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
T Vec3<T>::Dot(const Vec3<T>& a, const Vec3<T>& b)
{
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T> operator-(const Vec3<T>& lhs, const Vec3<T>& rhs)
{
    return (Vec3<T>(
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z
    ));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T> operator+(const Vec3<T>& lhs, const Vec3<T>& rhs)
{
    return (Vec3<T>(
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z
    ));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T>& operator+=(Vec3<T>& lhs, const Vec3<T>& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    return (lhs);
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T> operator*(const Vec3<T>& lhs, T rhs)
{
    return (Vec3<T>(
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs
    ));
}

} // namespace Ray

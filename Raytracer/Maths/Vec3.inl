///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Maths/Vec3.hpp"
#include <cmath>
#include <limits>
#include <type_traits>

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
    RAY_ASSERT(index < 3, RAY_ERROR_OVERFLOW);
    return (data[index]);
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
const T& Vec3<T>::operator[](Uint64 index) const
{
    RAY_ASSERT(index < 3, RAY_ERROR_OVERFLOW);
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
    T length = Vec3<T>::Length(vec);

    constexpr T epsilon = std::numeric_limits<T>::epsilon();

    if (std::abs(length) < epsilon)
    {
        return Vec3<T>(static_cast<T>(0));
    }

    T invLength = static_cast<T>(1) / length;
    return (Vec3<T>(
        vec.x * invLength,
        vec.y * invLength,
        vec.z * invLength
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
Vec3<T> Vec3<T>::Pow(const Vec3<T>& vec, const T& exp)
{
    return (Vec3<T>(
        std::pow(vec.x, exp),
        std::pow(vec.y, exp),
        std::pow(vec.z, exp)
    ));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
T Vec3<T>::Dot(const Vec3<T>& a, const Vec3<T>& b)
{
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T> Vec3<T>::Clamp(
    const Vec3<T>& vec,
    const Vec3<T>& min,
    const Vec3<T>& max
)
{
    return (Vec3<T>(
        std::max(min.x, std::min(vec.x, max.x)),
        std::max(min.y, std::min(vec.y, max.y)),
        std::max(min.z, std::min(vec.z, max.z))
    ));
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

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T> operator*(const Vec3<T>& lhs, const Vec3<T>& rhs)
{
    return (Vec3<T>(
        lhs.x * rhs.x,
        lhs.y * rhs.y,
        lhs.z * rhs.z
    ));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::ostream& operator<<(std::ostream& os, const Vec3<T>& vec)
{
    os << '(' << vec.x << ", " << vec.y << ", " << vec.z << ')';
    return (os);
}

} // namespace Ray

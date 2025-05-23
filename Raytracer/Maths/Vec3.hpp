///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Config.hpp"
#include "Maths/Vec4.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \tparam T
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class Vec3
{
public:
    ///////////////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////////////
    union
    {
        struct
        {
            T x;            //<!
            T y;            //<!
            T z;            //<!
        };
        T data[3];          //<!
    };

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    Vec3(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param all
    ///
    ///////////////////////////////////////////////////////////////////////////
    Vec3(const T& all);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param x
    /// \param y
    /// \param z
    ///
    ///////////////////////////////////////////////////////////////////////////
    Vec3(const T& x, const T& y, const T& z);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param other
    ///
    ///////////////////////////////////////////////////////////////////////////
    Vec3(const Vec4<T>& other);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param index
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    T& operator[](Uint64 index);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param index
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    const T& operator[](Uint64 index) const;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param a
    /// \param b
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    static Vec3<T> Min(const Vec3<T>& a, const Vec3<T>& b);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param a
    /// \param b
    /// \param c
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    static Vec3<T> Min(const Vec3<T>& a, const Vec3<T>& b, const Vec3<T>& c);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param a
    /// \param b
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    static Vec3<T> Max(const Vec3<T>& a, const Vec3<T>& b);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param a
    /// \param b
    /// \param c
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    static Vec3<T> Max(const Vec3<T>& a, const Vec3<T>& b, const Vec3<T>& c);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param a
    /// \param b
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    static Vec3<T> Cross(const Vec3<T>& a, const Vec3<T>& b);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param vec
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T Length(const Vec3<T>& vec);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param vec
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    static Vec3<T> Normalize(const Vec3<T>& vec);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param a
    /// \param b
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T Distance(const Vec3<T>& a, const Vec3<T>& b);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param a
    /// \param b
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T Dot(const Vec3<T>& a, const Vec3<T>& b);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param vec
    /// \param exp
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    static Vec3<T> Pow(const Vec3<T>& vec, const T& exp);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param vec
    /// \param min
    /// \param max
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    static Vec3<T> Clamp(
        const Vec3<T>& vec,
        const Vec3<T>& min,
        const Vec3<T>& max
    );
};

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param lhs
/// \param rhs
///
/// \return
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T> operator-(const Vec3<T>& lhs, const Vec3<T>& rhs);

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param lhs
/// \param rhs
///
/// \return
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T> operator+(const Vec3<T>& lhs, const Vec3<T>& rhs);

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param lhs
/// \param rhs
///
/// \return
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T>& operator+=(Vec3<T>& lhs, const Vec3<T>& rhs);

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param lhs
/// \param rhs
///
/// \return
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T> operator*(const Vec3<T>& lhs, T rhs);

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param lhs
/// \param rhs
///
/// \return
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T> operator*(const Vec3<T>& lhs, const Vec3<T>& rhs);

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param os
/// \param vec
///
/// \return
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::ostream& operator<<(std::ostream& os, const Vec3<T>& vec);

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
using Vec3i = Vec3<int>;
using Vec3u = Vec3<unsigned int>;
using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;

} // namespace Ray

///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////
#include "Maths/Vec3.inl"

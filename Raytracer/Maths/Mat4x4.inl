///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Maths/Mat4x4.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Mat4x4<T>::Mat4x4(void)
{
    for (Uint64 i = 0; i < 4; i++)
    {
        for (Uint64 j = 0; j < 4; j++)
        {
            if (i == j)
            {
                data[i][j] = T(1);
            }
            else
            {
                data[i][j] = T(0);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
T (&Mat4x4<T>::operator[](Uint64 index))[4]
{
    return (data[index]);
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
const T (&Mat4x4<T>::operator[](Uint64 index) const)[4]
{
    return (data[index]);
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Mat4x4<T> Mat4x4<T>::QuaternionToMatrix(const Quaternion<T>& quaternion)
{
    Mat4x4<T> result;

    const T x2 = quaternion.x + quaternion.x;
    const T y2 = quaternion.y + quaternion.y;
    const T z2 = quaternion.z + quaternion.z;

    const T xx = quaternion.x * x2;
    const T xy = quaternion.x * y2;
    const T xz = quaternion.x * z2;

    const T yy = quaternion.y * y2;
    const T yz = quaternion.y * z2;
    const T zz = quaternion.z * z2;

    const T wx = quaternion.w * x2;
    const T wy = quaternion.w * y2;
    const T wz = quaternion.w * z2;

    result[0][0] = T(1) - (yy + zz);
    result[0][1] = xy + wz;
    result[0][2] = xz - wy;
    result[0][3] = T(0);

    result[1][0] = xy - wz;
    result[1][1] = T(1) - (xx + zz);
    result[1][2] = yz + wx;
    result[1][3] = T(0);

    result[2][0] = xz + wy;
    result[2][1] = yz - wx;
    result[2][2] = T(1) - (xx + yy);
    result[2][3] = T(0);

    result[3][0] = T(0);
    result[3][1] = T(0);
    result[3][2] = T(0);
    result[3][3] = T(1);

    return (result);
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Mat4x4<T> operator*(const Mat4x4<T>& lhs, const Mat4x4<T>& rhs)
{
    Mat4x4<T> result;

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            result[row][col] = T(0);

            for (int i = 0; i < 4; i++)
            {
                result[row][col] += lhs[row][i] * rhs[i][col];
            }
        }
    }

    return (result);
}

} // namespace Ray

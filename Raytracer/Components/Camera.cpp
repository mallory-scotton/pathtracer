///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Components/Camera.hpp"
#include <cstring>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

// FIXME: C FUNCTION + MATRIX 4x4 AS C POINTERS
///////////////////////////////////////////////////////////////////////////////
void Frustum(
    float left,
    float right,
    float bottom,
    float top,
    float znear,
    float zfar,
    float* m16
) {
    float temp, temp2, temp3, temp4;
    temp = 2.0f * znear;
    temp2 = right - left;
    temp3 = top - bottom;
    temp4 = zfar - znear;
    m16[0] = temp / temp2;
    m16[1] = 0.0;
    m16[2] = 0.0;
    m16[3] = 0.0;
    m16[4] = 0.0;
    m16[5] = temp / temp3;
    m16[6] = 0.0;
    m16[7] = 0.0;
    m16[8] = (right + left) / temp2;
    m16[9] = (top + bottom) / temp3;
    m16[10] = (-zfar - znear) / temp4;
    m16[11] = -1.0f;
    m16[12] = 0.0;
    m16[13] = 0.0;
    m16[14] = (-temp * zfar) / temp4;
    m16[15] = 0.0;
}

// FIXME: C FUNCTION + MATRIX 4x4 AS C POINTERS
///////////////////////////////////////////////////////////////////////////////
void Perspective(
    float fovyInDegrees,
    float aspectRatio,
    float znear,
    float zfar,
    float* m16
) {
    float ymax, xmax;
    ymax = znear * tanf(fovyInDegrees * 3.141592f / 180.0f);
    xmax = ymax * aspectRatio;
    Frustum(-xmax, xmax, -ymax, ymax, znear, zfar, m16);
}

// FIXME: C FUNCTION + VEC3F AS C POINTERS
///////////////////////////////////////////////////////////////////////////////
void Cross(const float* a, const float* b, float* r)
{
    r[0] = a[1] * b[2] - a[2] * b[1];
    r[1] = a[2] * b[0] - a[0] * b[2];
    r[2] = a[0] * b[1] - a[1] * b[0];
}

// FIXME: C FUNCTION + VEC3F AS C POINTERS
///////////////////////////////////////////////////////////////////////////////
float Dot(const float* a, const float* b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

// FIXME: C FUNCTION + VEC3F AS C POINTERS
///////////////////////////////////////////////////////////////////////////////
void Normalize(const float* a, float* r)
{
    float il = 1.f / (sqrtf(Dot(a, a)) + FLT_EPSILON);
    r[0] = a[0] * il;
    r[1] = a[1] * il;
    r[2] = a[2] * il;
}

// FIXME: C FUNCTION + VEC3F AS C POINTERS + MATRIX 4x4 AS C POINTERS
///////////////////////////////////////////////////////////////////////////////
void LookAt(const float* eye, const float* at, const float* up, float* m16)
{
    float X[3], Y[3], Z[3], tmp[3];

    tmp[0] = eye[0] - at[0];
    tmp[1] = eye[1] - at[1];
    tmp[2] = eye[2] - at[2];

    Normalize(tmp, Z);
    Normalize(up, Y);

    Cross(Y, Z, tmp);
    Normalize(tmp, X);

    Cross(Z, X, tmp);
    Normalize(tmp, Y);

    m16[0] = X[0];
    m16[1] = Y[0];
    m16[2] = Z[0];
    m16[3] = 0.0f;
    m16[4] = X[1];
    m16[5] = Y[1];
    m16[6] = Z[1];
    m16[7] = 0.0f;
    m16[8] = X[2];
    m16[9] = Y[2];
    m16[10] = Z[2];
    m16[11] = 0.0f;
    m16[12] = -Dot(X, eye);
    m16[13] = -Dot(Y, eye);
    m16[14] = -Dot(Z, eye);
    m16[15] = 1.0f;
}

///////////////////////////////////////////////////////////////////////////////
Camera::Camera(Vec3f eye, Vec3f lookat, float fov)
    : position(eye)
    , focalDist(0.1f)
    , aperture(0.f)
    , fov(Math::Radians(fov))
    , worldUp(0.f, 1.f, 0.f)
    , pivot(lookat)
    , radius(Vec3f::Distance(eye, lookat))
{
    Vec3f dir = Vec3f::Normalize(pivot - position);
    pitch = Math::Degrees(asin(dir.y));
    yaw = Math::Degrees(atan2(dir.z, dir.x));

    UpdateCamera();
}

///////////////////////////////////////////////////////////////////////////////
Camera::Camera(const Camera& other)
{
    *this = other;
}

///////////////////////////////////////////////////////////////////////////////
Camera& Camera::operator=(const Camera& other)
{
    ptrdiff_t l = (unsigned char*)&isMoving - (unsigned char*)&position.x;
    isMoving = memcmp(&position.x, &other.position.x, l) != 0;
    memcpy(&position.x, &other.position.x, l);
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
void Camera::OffsetOrientation(float dx, float dy)
{
    pitch -= dy;
    yaw += dx;
    UpdateCamera();
}

///////////////////////////////////////////////////////////////////////////////
void Camera::Strafe(float dx, float dy)
{
    Vec3f translation = right * -dx + up * dy;
    pivot = pivot + translation;
    UpdateCamera();
}

///////////////////////////////////////////////////////////////////////////////
void Camera::SetRadius(float dr)
{
    radius += dr;
    UpdateCamera();
}

///////////////////////////////////////////////////////////////////////////////
void Camera::SetPosition(const Vec3f& position)
{
    this->position = position;

    radius = Vec3f::Distance(position, pivot);

    Vec3f dir = Vec3f::Normalize(pivot - position);
    pitch = Math::Degrees(asin(dir.y));
    yaw = Math::Degrees(atan2(dir.z, dir.x));

    UpdateCamera();
}

///////////////////////////////////////////////////////////////////////////////
void Camera::SetLookAt(const Vec3f& lookAt)
{
    this->pivot = lookAt;
    radius = Vec3f::Distance(position, pivot);

    if (radius > FLT_EPSILON)
    {
        Vec3f dir = Vec3f::Normalize(pivot - position);
        pitch = Math::Degrees(asin(dir.y));
        yaw = Math::Degrees(atan2(dir.z, dir.x));
    }

    UpdateCamera();
}

///////////////////////////////////////////////////////////////////////////////
void Camera::SetFov(float val)
{
    fov = Math::Radians(val);
}

///////////////////////////////////////////////////////////////////////////////
void Camera::SetUniforms(UniquePtr<Shader>& shader) const
{
    shader->Uniform("camera.position", position);
    shader->Uniform("camera.right", right);
    shader->Uniform("camera.up", up);
    shader->Uniform("camera.forward", forward);
    shader->Uniform("camera.fov", fov);
    shader->Uniform("camera.focalDist", focalDist);
    shader->Uniform("camera.aperture", aperture);
}

///////////////////////////////////////////////////////////////////////////////
void Camera::UpdateCamera(void)
{
    Vec3f forward_temp;
    forward_temp.x = std::cos(Math::Radians(yaw)) *
        std::cos(Math::Radians(pitch));
    forward_temp.y = std::sin(Math::Radians(pitch));
    forward_temp.z = std::sin(Math::Radians(yaw)) *
        std::cos(Math::Radians(pitch));

    forward = Vec3f::Normalize(forward_temp);
    position = pivot + (forward * -1.0f) * radius;

    right = Vec3f::Normalize(Vec3f::Cross(forward, worldUp));
    up = Vec3f::Normalize(Vec3f::Cross(right, forward));
}

// FIXME: MATRIX 4x4 AS C POINTERS
///////////////////////////////////////////////////////////////////////////////
void Camera::ComputeViewProjectionMatrix(
    float* view,
    float* projection,
    float ratio
) {
    Vec3f at = position + forward;
    LookAt(&position.x, &at.x, &up.x, view);
    const float fov_v = (1.f / ratio) * tanf(fov / 2.f);
    Perspective(Math::Degrees(fov_v), ratio, 0.1f, 1000.f, projection);
}

}  // namespace Ray

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Renderer/Camera.hpp"
#include "Maths/Utils.hpp"
#include "Utils/OpenGL.hpp"
#include <cstring>

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
Camera::Camera(const Vec3f& eye, const Vec3f& lookAt, float fov)
    : mPosition(eye)
    , mFocalDistance(0.1f)
    , mAperture(0.f)
    , mFOV(Math::Radians(fov))
    , mWorldUp(0.f, 1.f, 0.f)
    , mPivot(lookAt)
    , mRadius(Vec3f::Distance(eye, lookAt))
{
    Vec3f direction = Vec3f::Normalize(mPivot - mPosition);

    mPitch = Math::Degrees(std::asin(direction.y));
    mYaw = Math::Degrees(std::atan2(direction.z, direction.x));

    Update();
}

///////////////////////////////////////////////////////////////////////////////
Camera::Camera(const Map<String, Vector<String>>& props)
    : mFocalDistance(0.1f)
    , mAperture(0.f)
    , mWorldUp(0.f, 1.f, 0.f)
{
    for (const auto& [key, values] : props)
    {
        Uint64 n = values.size();

        if (key == "position" && n == 3)
        {
            mPosition = Vec3f(
                std::stof(values[0]),
                std::stof(values[1]),
                std::stof(values[2])
            );
        }
        else if (key == "lookAt" && n == 3)
        {
            mPivot = Vec3f(
                std::stof(values[0]),
                std::stof(values[1]),
                std::stof(values[2])
            );
        }
        else if (key == "aperture" && n == 1)
        {
            mAperture = std::stof(values[0]);
        }
        else if (key == "focalDistance" && n == 1)
        {
            mFocalDistance = std::stof(values[0]);
        }
        else if (key == "fov" && n == 1)
        {
            mFOV = Math::Radians(std::stof(values[0]));
        }
        else if (key == "matrix" && n == 16)
        {
            // TODO: Add matrix calculation
        }
        else
        {
            RAY_WARN("\"" + key + "\" is not a valid Camera attributes");
        }
    }

    Vec3f direction = Vec3f::Normalize(mPivot - mPosition);

    mRadius = Vec3f::Distance(mPosition, mPivot);
    mPitch = Math::Degrees(std::asin(direction.y));
    mYaw = Math::Degrees(std::atan2(direction.z, direction.x));

    Update();
}

///////////////////////////////////////////////////////////////////////////////
Camera::Camera(const Camera& other)
{
    *this = other;
}

///////////////////////////////////////////////////////////////////////////////
Camera& Camera::operator=(const Camera& other)
{
    ptrdiff_t length =
        (unsigned char*)&mIsMoving -
        (unsigned char*)&mPosition.x;
    mIsMoving = memcmp(&mPosition.x, &other.mPosition.x, length) != 0;
    memcpy(&mPosition.x, &other.mPosition.x, length);
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
void Camera::Update(void)
{
    Vec3f temp(
        std::cos(Math::Radians(mYaw)) * std::cos(Math::Radians(mPitch)),
        std::sin(Math::Radians(mPitch)),
        std::sin(Math::Radians(mYaw)) * std::cos(Math::Radians(mPitch))
    );

    mForward = Vec3f::Normalize(temp);
    mPosition = mPivot + (mForward * -1.f) * mRadius;

    mRight = Vec3f::Normalize(Vec3f::Cross(mForward, mWorldUp));
    mUp = Vec3f::Normalize(Vec3f::Cross(mRight, mForward));
}

///////////////////////////////////////////////////////////////////////////////
void Camera::SetFOV(float fov)
{
    mFOV = Math::Radians(fov);
}

///////////////////////////////////////////////////////////////////////////////
void Camera::SetRadius(float radius)
{
    mRadius += radius;
    Update();
}

///////////////////////////////////////////////////////////////////////////////
void Camera::Strafe(const Vec2f& direction)
{
    Vec3f translation = mRight * -direction.x + mUp * direction.y;

    mPivot += translation;
    Update();
}

///////////////////////////////////////////////////////////////////////////////
void Camera::OffsetOrientation(const Vec2f& direction)
{
    mPitch -= direction.y;
    mYaw += direction.x;
    Update();
}

///////////////////////////////////////////////////////////////////////////////
void Camera::UpdateUniforms(UniquePtr<Shader>& shader) const
{
    shader->Uniform("camera.position", mPosition);
    shader->Uniform("camera.right", mRight);
    shader->Uniform("camera.up", mUp);
    shader->Uniform("camera.forward", mForward);
    shader->Uniform("camera.fov", mFOV);
    shader->Uniform("camera.focalDist", mFocalDistance);
    shader->Uniform("camera.aperture", mAperture);
}

} // namespace Ray

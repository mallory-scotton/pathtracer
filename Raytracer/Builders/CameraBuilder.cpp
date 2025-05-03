///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Builders/CameraBuilder.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
CameraBuilder::CameraBuilder(void)
    : m_camera(Vec3f(0.f), Vec3f(0.f), 60.f)
{}

///////////////////////////////////////////////////////////////////////////////
Camera CameraBuilder::Build(void)
{
    return (m_camera);
}

///////////////////////////////////////////////////////////////////////////////
CameraBuilder& CameraBuilder::FromConfiguration(
    const LibConfig::Setting& config
)
{
    Vec3f position, lookAt;
    float fov, aperture = 0.f, focalDist = 1.f;

    config.Value("position", position);
    config.Value("lookat", lookAt);
    config.Value("fov", fov);
    config.Value("aperture", aperture);
    config.Value("focalDist", focalDist);

    m_camera = Camera(position, lookAt, fov);
    m_camera.aperture = aperture;
    m_camera.focalDist = focalDist;

    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
CameraBuilder& CameraBuilder::SetPosition(const Vec3f& position)
{
    m_camera.SetPosition(position);
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
CameraBuilder& CameraBuilder::SetLookAt(const Vec3f& lookAt)
{
    m_camera.SetLookAt(lookAt);
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
CameraBuilder& CameraBuilder::SetFov(float fov)
{
    m_camera.fov = Math::Radians(fov);
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
CameraBuilder& CameraBuilder::SetAperture(float aperture)
{
    m_camera.aperture = aperture;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
CameraBuilder& CameraBuilder::SetFocalDistance(float distance)
{
    m_camera.focalDist = distance;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
CameraBuilder& CameraBuilder::SetTransformMatrix(const Mat4x4f& matrix)
{
   Vec3f forward = Vec3f(matrix[2][0], matrix[2][1], matrix[2][2]);
   Vec3f position = Vec3f(matrix[3][0], matrix[3][1], matrix[3][2]);
   Vec3f lookAt = position + forward;

   m_camera.forward = forward;
   SetPosition(position);
   SetLookAt(lookAt);
   return (*this);
}

} // namespace Ray

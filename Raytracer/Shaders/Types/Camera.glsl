///////////////////////////////////////////////////////////////////////////////
/// \brief Camera definition for ray generation
///
///////////////////////////////////////////////////////////////////////////////
struct Camera
{
  vec3 up;              //<! Up vector defining camera orientation
  vec3 right;           //<! Right vector defining camera orientation
  vec3 forward;         //<! Forward vector defining camera orientation
  vec3 position;        //<! Position of the camera in world space
  float fov;            //<! Field of view in radians
  float focalDist;      //<! Focal distance for depth of field
  float aperture;       //<! Aperture size for depth of field effect
};

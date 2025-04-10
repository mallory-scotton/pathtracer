///////////////////////////////////////////////////////////////////////////////
/// \brief Light source definition
///
///////////////////////////////////////////////////////////////////////////////
struct Light
{
  vec3 position;        //<! Position of the light in world space
  vec3 emission;        //<! Color and intensity of light emission
  vec3 u;               //<! First tangent vector for area lights
  vec3 v;               //<! Second tangent vector for area lights
  float radius;         //<! Radius for spherical lights
  float area;           //<! Surface area of the light for importance sampling
  float type;           //<! Type of light
};

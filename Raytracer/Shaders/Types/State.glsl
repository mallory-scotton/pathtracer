///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Types/Material.glsl"
#include "Types/Medium.glsl"

///////////////////////////////////////////////////////////////////////////////
/// \brief Ray intersection state information
///
///////////////////////////////////////////////////////////////////////////////
struct State
{
  int depth;            //<! Current recursion depth in path tracing
  float eta;            //<! Relative index of refraction at interface
  float hitDist;        //<! Distance to intersection point

  vec3 fhp;             //<! First hit point in world space
  vec3 normal;          //<! Surface normal at hit point
  vec3 ffnormal;        //<! Forward-facing normal at hit point
  vec3 tangent;         //<! Tangent vector for normal mapping
  vec3 bitangent;       //<! Bitangent vector for normal mapping

  bool isEmitter;       //<! Flag indicating if hit surface is emissive

  vec2 texCoord;        //<! Texture coordinates at hit point
  int matID;            //<! Material ID for the hit surface
  Material mat;         //<! Material properties at hit point
  Medium medium;        //<! Current participating medium
};

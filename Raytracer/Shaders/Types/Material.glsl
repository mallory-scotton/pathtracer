///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Types/Medium.glsl"

///////////////////////////////////////////////////////////////////////////////
/// \brief Material definition using Disney principled BRDF
///
///////////////////////////////////////////////////////////////////////////////
struct Material
{
  vec3 baseColor;             //<! Base color/albedo of the material
  float opacity;              //<! Opacity for transparent materials
  int alphaMode;              //<! Alpha blending mode
  float alphaCutOff;          //<! Threshold for alpha masking
  vec3 emission;              //<! Self-emission color and intensity
  float anisotropic;          //<! Anisotropy factor for highlights
  float metallic;             //<! Metallic factor
  float roughness;            //<! Surface roughness
  float subsurface;           //<! Subsurface scattering intensity
  float specularTint;         //<! Tint for specular highlights
  float sheen;                //<! Sheen intensity for cloth-like materials
  float sheenTint;            //<! Tint factor for sheen
  float clearcoat;            //<! Clearcoat layer intensity
  float clearcoatRoughness;   //<! Roughness of the clearcoat layer
  float specTrans;            //<! Specular transmission factor
  float ior;                  //<! Index of refraction
  float ax;                   //<! Anisotropic roughness in x direction
  float ay;                   //<! Anisotropic roughness in y direction
  Medium medium;              //<! Associated participating medium
};

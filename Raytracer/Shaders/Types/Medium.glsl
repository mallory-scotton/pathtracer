///////////////////////////////////////////////////////////////////////////////
/// \brief Participating medium for volumetric effects
///
///////////////////////////////////////////////////////////////////////////////
struct Medium
{
  int type;             //<! Medium type
  float density;        //<! Density coefficient for scattering
  vec3 color;           //<! Absorption/scattering color
  float anisotropy;     //<! Anisotropy factor for phase function
};

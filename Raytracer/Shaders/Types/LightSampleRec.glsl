///////////////////////////////////////////////////////////////////////////////
/// \brief Record containing light sampling information
///
///////////////////////////////////////////////////////////////////////////////
struct LightSampleRec
{
  vec3 normal;          //<! Surface normal at the sampled point on the light
  vec3 emission;        //<! Emitted radiance at the sampled point
  vec3 direction;       //<! Direction from shading point to light
  float dist;           //<! Distance from shading point to light
  float pdf;            //<! Probability density function value for the sample
};

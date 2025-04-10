///////////////////////////////////////////////////////////////////////////////
/// \brief Record containing scattering sample information
///
///////////////////////////////////////////////////////////////////////////////
struct ScatterSampleRec
{
  vec3 L;               //<! Sampled direction for light transport
  vec3 f;               //<! BSDF evaluation for the sampled direction
  float pdf;            //<! Probability density function value for the sample
};

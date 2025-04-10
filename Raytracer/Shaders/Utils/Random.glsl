///////////////////////////////////////////////////////////////////////////////
// Internal RNG states
///////////////////////////////////////////////////////////////////////////////
uvec4 seed;           //<! The current RNG state
ivec2 pixel;          //<! 2D pixel coordinates for the current fragment

///////////////////////////////////////////////////////////////////////////////
/// \brief Initializes the random number generator with position and frame data
///
/// \param p 2D position vector
/// \param frame Current frame number to vary randomness over time
///
///////////////////////////////////////////////////////////////////////////////
void InitRNG(vec2 p, int frame)
{
  pixel = ivec2(p);
  seed = uvec4(p, uint(frame), uint(p.x) + uint(p.y));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Advances the state of the 4D Permuted Congruential Generator
///
/// \param v Reference to the 4D state vector to be updated
///
///////////////////////////////////////////////////////////////////////////////
void AdvancePRNG4D(inout uvec4 v)
{
  v = v * 1664525u + 1013904223u;
  v.x += v.y * v.w; v.y += v.z * v.x; v.z += v.x * v.y; v.w += v.y * v.z;
  v = v ^ (v >> 16u);
  v.x += v.y * v.w; v.y += v.z * v.x; v.z += v.x * v.y; v.w += v.y * v.z;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Generates a random float value between 0.0 and 1.0
///
/// \return Random floating-point value in range [0.0, 1.0)
///
///////////////////////////////////////////////////////////////////////////////
float Rand()
{
  AdvancePRNG4D(seed);
  return (float(seed.x) / float(0xffffffffu));
}

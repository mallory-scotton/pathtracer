///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param state
/// \param V
/// \param N
/// \param L
/// \param pdf
///
/// \return
///
///////////////////////////////////////////////////////////////////////////////
vec3 LambertSample(
    inout State state,
    vec3 V,
    vec3 N,
    inout vec3 L,
    inout float pdf
)
{
    float r1 = rand();
    float r2 = rand();

    vec3 T, B;
    Onb(N, T, B);

    L = CosineSampleHemisphere(r1, r2);
    L = T * L.x + B * L.y + N * L.z;

    pdf = dot(N, L) * (1.0 / PI);

    return ((1.0 / PI) * state.mat.baseColor * dot(N, L));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param state
/// \param V
/// \param N
/// \param L
/// \param pdf
///
///////////////////////////////////////////////////////////////////////////////
vec3 LambertEval(State state, vec3 V, vec3 N, vec3 L, inout float pdf)
{
    pdf = dot(N, L) * (1.0 / PI);

    return ((1.0 / PI) * state.mat.baseColor * dot(N, L));
}

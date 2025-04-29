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
vec3 DisneyEval(State state, vec3 V, vec3 N, vec3 L, out float pdf);

vec3 ToWorld(vec3 X, vec3 Y, vec3 Z, vec3 V)
{
    return (V.x * X + V.y * Y + V.z * Z);
}

vec3 ToLocal(vec3 X, vec3 Y, vec3 Z, vec3 V)
{
    return (vec3(dot(V, X), dot(V, Y), dot(V, Z)));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param mat
/// \param eta
/// \param F0
/// \param Csheen
/// \param Cspec0
///
/// \return
///
///////////////////////////////////////////////////////////////////////////////
void TintColors(
    Material mat,
    float eta,
    out float F0,
    out vec3 Csheen,
    out vec3 Cspec0
)
{
    float lum = Luminance(mat.baseColor);
    vec3 ctint = lum > 0.0 ? mat.baseColor / lum : vec3(1.0);

    F0 = (1.0 - eta) / (1.0 + eta);
    F0 *= F0;

    Cspec0 = F0 * mix(vec3(1.0), ctint, mat.specularTint);
    Csheen = mix(vec3(1.0), ctint, mat.sheenTint);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param mat
/// \param Csheen
/// \param V
/// \param L
/// \param H
/// \param pdf
///
/// \return
///
///////////////////////////////////////////////////////////////////////////////
vec3 EvalDisneyDiffuse(
    Material mat,
    vec3 Csheen,
    vec3 V,
    vec3 L,
    vec3 H,
    out float pdf
)
{
    pdf = 0.0;
    if (L.z <= 0.0)
    {
        return (vec3(0.0));
    }

    float LDotH = dot(L, H);

    float Rr = 2.0 * mat.roughness * LDotH * LDotH;

    float FL = SchlickWeight(L.z);
    float FV = SchlickWeight(V.z);
    float Fretro = Rr * (FL + FV + FL * FV * (Rr - 1.0));
    float Fd = (1.0 - 0.5 * FL) * (1.0 - 0.5 * FV);

    float Fss90 = 0.5 * Rr;
    float Fss = mix(1.0, Fss90, FL) * mix(1.0, Fss90, FV);
    float ss = 1.25 * (Fss * (1.0 / (L.z + V.z) - 0.5) + 0.5);

    float FH = SchlickWeight(LDotH);
    vec3 Fsheen = FH * mat.sheen * Csheen;

    pdf = L.z * INV_PI;
    return (INV_PI * mat.baseColor * mix(Fd + Fretro, ss, mat.subsurface) + Fsheen);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param mat
/// \param V
/// \param L
/// \param H
/// \param F
/// \param pdf
///
/// \return
///
///////////////////////////////////////////////////////////////////////////////
vec3 EvalMicrofacetReflection(
    Material mat,
    vec3 V,
    vec3 L,
    vec3 H,
    vec3 F,
    out float pdf
)
{
    pdf = 0.0;
    if (L.z <= 0.0)
    {
        return (vec3(0.0));
    }

    float D = GTR2Aniso(H.z, H.x, H.y, mat.ax, mat.ay);
    float G1 = SmithGAniso(abs(V.z), V.x, V.y, mat.ax, mat.ay);
    float G2 = G1 * SmithGAniso(abs(L.z), L.x, L.y, mat.ax, mat.ay);

    pdf = G1 * D / (4.0 * V.z);
    return (F * D * G2 / (4.0 * L.z * V.z));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param mat
/// \param eta
/// \param V
/// \param L
/// \param H
/// \param F
/// \param pdf
///
/// \return
///
///////////////////////////////////////////////////////////////////////////////
vec3 EvalMicrofacetRefraction(
    Material mat,
    float eta,
    vec3 V,
    vec3 L,
    vec3 H,
    vec3 F,
    out float pdf
)
{
    pdf = 0.0;
    if (L.z >= 0.0)
    {
        return (vec3(0.0));
    }

    float LDotH = dot(L, H);
    float VDotH = dot(V, H);

    float D = GTR2Aniso(H.z, H.x, H.y, mat.ax, mat.ay);
    float G1 = SmithGAniso(abs(V.z), V.x, V.y, mat.ax, mat.ay);
    float G2 = G1 * SmithGAniso(abs(L.z), L.x, L.y, mat.ax, mat.ay);
    float denom = LDotH + VDotH * eta;
    denom *= denom;
    float eta2 = eta * eta;
    float jacobian = abs(LDotH) / denom;

    pdf = G1 * max(0.0, VDotH) * D * jacobian / V.z;

    return (
        pow(mat.baseColor, vec3(0.5)) *
        (1.0 - F) * D * G2 * abs(VDotH) *
        jacobian * eta2 / abs(L.z * V.z)
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param state
/// \param V
/// \param L
/// \param H
/// \param pdf
///
/// \return
///
///////////////////////////////////////////////////////////////////////////////
vec3 EvalClearcoat(Material mat, vec3 V, vec3 L, vec3 H, out float pdf)
{
    pdf = 0.0;
    if (L.z <= 0.0)
    {
        return (vec3(0.0));
    }

    float VDotH = dot(V, H);

    float F = mix(0.04, 1.0, SchlickWeight(VDotH));
    float D = GTR1(H.z, mat.clearcoatRoughness);
    float G = SmithG(L.z, 0.25) * SmithG(V.z, 0.25);
    float jacobian = 1.0 / (4.0 * VDotH);

    pdf = D * H.z * jacobian;
    return (vec3(F) * D * G);
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
/// \return
///
///////////////////////////////////////////////////////////////////////////////
vec3 DisneySample(State state, vec3 V, vec3 N, out vec3 L, out float pdf)
{
    pdf = 0.0;

    float r1 = rand();
    float r2 = rand();

    vec3 T, B;
    Onb(N, T, B);

    V = ToLocal(T, B, N, V);

    vec3 Csheen, Cspec0;
    float F0;
    TintColors(state.mat, state.eta, F0, Csheen, Cspec0);

    float dielectricWt = (1.0 - state.mat.metallic) * (1.0 - state.mat.specTrans);
    float metalWt = state.mat.metallic;
    float glassWt = (1.0 - state.mat.metallic) * state.mat.specTrans;

    float schlickWt = SchlickWeight(V.z);

    float diffPr = dielectricWt * Luminance(state.mat.baseColor);
    float dielectricPr = dielectricWt * Luminance(mix(Cspec0, vec3(1.0), schlickWt));
    float metalPr = metalWt * Luminance(mix(state.mat.baseColor, vec3(1.0), schlickWt));
    float glassPr = glassWt;
    float clearCtPr = 0.25 * state.mat.clearcoat;

    float invTotalWt = 1.0 / (diffPr + dielectricPr + metalPr + glassPr + clearCtPr);
    diffPr *= invTotalWt;
    dielectricPr *= invTotalWt;
    metalPr *= invTotalWt;
    glassPr *= invTotalWt;
    clearCtPr *= invTotalWt;

    float cdf[5];
    cdf[0] = diffPr;
    cdf[1] = cdf[0] + dielectricPr;
    cdf[2] = cdf[1] + metalPr;
    cdf[3] = cdf[2] + glassPr;
    cdf[4] = cdf[3] + clearCtPr;

    float r3 = rand();

    if (r3 < cdf[0])
    {
        L = CosineSampleHemisphere(r1, r2);
    }
    else if (r3 < cdf[2])
    {
        vec3 H = SampleGGXVNDF(V, state.mat.ax, state.mat.ay, r1, r2);

        if (H.z < 0.0)
        {
            H = -H;
        }

        L = normalize(reflect(-V, H));
    }
    else if (r3 < cdf[3])
    {
        vec3 H = SampleGGXVNDF(V, state.mat.ax, state.mat.ay, r1, r2);
        float F = DielectricFresnel(abs(dot(V, H)), state.eta);

        if (H.z < 0.0)
        {
            H = -H;
        }

        r3 = (r3 - cdf[2]) / (cdf[3] - cdf[2]);

        if (r3 < F)
        {
            L = normalize(reflect(-V, H));
        }
        else
        {
            L = normalize(refract(-V, H, state.eta));
        }
    }
    else
    {
        vec3 H = SampleGTR1(state.mat.clearcoatRoughness, r1, r2);

        if (H.z < 0.0)
        {
            H = -H;
        }

        L = normalize(reflect(-V, H));
    }

    L = ToWorld(T, B, N, L);
    V = ToWorld(T, B, N, V);

    return (DisneyEval(state, V, N, L, pdf));
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
/// \return
///
///////////////////////////////////////////////////////////////////////////////
vec3 DisneyEval(State state, vec3 V, vec3 N, vec3 L, out float pdf)
{
    pdf = 0.0;
    vec3 f = vec3(0.0);

    vec3 T, B;
    Onb(N, T, B);

    V = ToLocal(T, B, N, V);
    L = ToLocal(T, B, N, L);

    vec3 H;
    if (L.z > 0.0)
    {
        H = normalize(L + V);
    }
    else
    {
        H = normalize(L + V * state.eta);
    }

    if (H.z < 0.0)
    {
        H = -H;
    }

    vec3 Csheen, Cspec0;
    float F0;
    TintColors(state.mat, state.eta, F0, Csheen, Cspec0);

    float dielectricWt = (1.0 - state.mat.metallic) * (1.0 - state.mat.specTrans);
    float metalWt = state.mat.metallic;
    float glassWt = (1.0 - state.mat.metallic) * state.mat.specTrans;

    float schlickWt = SchlickWeight(V.z);

    float diffPr = dielectricWt * Luminance(state.mat.baseColor);
    float dielectricPr = dielectricWt * Luminance(mix(Cspec0, vec3(1.0), schlickWt));
    float metalPr = metalWt * Luminance(mix(state.mat.baseColor, vec3(1.0), schlickWt));
    float glassPr = glassWt;
    float clearCtPr = 0.25 * state.mat.clearcoat;

    float invTotalWt = 1.0 / (diffPr + dielectricPr + metalPr + glassPr + clearCtPr);
    diffPr *= invTotalWt;
    dielectricPr *= invTotalWt;
    metalPr *= invTotalWt;
    glassPr *= invTotalWt;
    clearCtPr *= invTotalWt;

    bool reflect = L.z * V.z > 0;

    float tmpPdf = 0.0;
    float VDotH = abs(dot(V, H));

    if (diffPr > 0.0 && reflect)
    {
        f += EvalDisneyDiffuse(state.mat, Csheen, V, L, H, tmpPdf) * dielectricWt;
        pdf += tmpPdf * diffPr;
    }

    if (dielectricPr > 0.0 && reflect)
    {
        float F = (DielectricFresnel(VDotH, 1.0 / state.mat.ior) - F0) / (1.0 - F0);

        f += EvalMicrofacetReflection(state.mat, V, L, H, mix(Cspec0, vec3(1.0), F), tmpPdf) * dielectricWt;
        pdf += tmpPdf * dielectricPr;
    }

    if (metalPr > 0.0 && reflect)
    {
        vec3 F = mix(state.mat.baseColor, vec3(1.0), SchlickWeight(VDotH));

        f += EvalMicrofacetReflection(state.mat, V, L, H, F, tmpPdf) * metalWt;
        pdf += tmpPdf * metalPr;
    }

    if (glassPr > 0.0)
    {
        float F = DielectricFresnel(VDotH, state.eta);

        if (reflect)
        {
            f += EvalMicrofacetReflection(state.mat, V, L, H, vec3(F), tmpPdf) * glassWt;
            pdf += tmpPdf * glassPr * F;
        }
        else
        {
            f += EvalMicrofacetRefraction(state.mat, state.eta, V, L, H, vec3(F), tmpPdf) * glassWt;
            pdf += tmpPdf * glassPr * (1.0 - F);
        }
    }

    if (clearCtPr > 0.0 && reflect)
    {
        f += EvalClearcoat(state.mat, V, L, H, tmpPdf) * 0.25 * state.mat.clearcoat;
        pdf += tmpPdf * clearCtPr;
    }

    return (f * abs(L.z));
}

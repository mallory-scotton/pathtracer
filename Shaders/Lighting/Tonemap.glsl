///////////////////////////////////////////////////////////////////////////////
// OpenGL Version
///////////////////////////////////////////////////////////////////////////////
#version 330

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
out vec4 outCol;
in vec2 TexCoords;

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
uniform sampler2D pathTraceTexture;
uniform float invSampleCounter;
uniform bool enableTonemap;
uniform bool enableAces;
uniform bool simpleAcesFit;
uniform vec3 backgroundCol;

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Globals.glsl"

///////////////////////////////////////////////////////////////////////////////
mat3 ACESInputMat = mat3
(
    vec3(0.59719, 0.35458, 0.04823),
    vec3(0.07600, 0.90834, 0.01566),
    vec3(0.02840, 0.13383, 0.83777)
);

///////////////////////////////////////////////////////////////////////////////
mat3 ACESOutputMat = mat3
(
    vec3(1.60475, -0.53108, -0.07367),
    vec3(-0.10208, 1.10813, -0.00605),
    vec3(-0.00327, -0.07276, 1.07602)
);

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param v
///
/// \return
///
///////////////////////////////////////////////////////////////////////////////
vec3 RRTAndODTFit(vec3 v)
{
    vec3 a = v * (v + 0.0245786f) - 0.000090537f;
    vec3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return (a / b);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param color
///
/// \return
///
///////////////////////////////////////////////////////////////////////////////
vec3 ACESFitted(vec3 color)
{
    color = color * ACESInputMat;

    color = RRTAndODTFit(color);

    color = color * ACESOutputMat;

    color = clamp(color, 0.0, 1.0);

    return (color);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param c
///
/// \return
///
///////////////////////////////////////////////////////////////////////////////
vec3 ACES(in vec3 c)
{
    float a = 2.51f;
    float b = 0.03f;
    float y = 2.43f;
    float d = 0.59f;
    float e = 0.14f;

    return (clamp((c * (a * c + b)) / (c * (y * c + d) + e), 0.0, 1.0));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param c
/// \param limit
///
/// \return
///
///////////////////////////////////////////////////////////////////////////////
vec3 Tonemap(in vec3 c, float limit)
{
    return (c * 1.0 / (1.0 + Luminance(c) / limit));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
void main()
{
    vec4 col = texture(pathTraceTexture, TexCoords) * invSampleCounter;
    vec3 color = col.rgb;
    float alpha = col.a;

    if (enableTonemap)
    {
        if (enableAces)
        {
            if (simpleAcesFit)
            {
                color = ACES(color);
            }
            else
            {
                color = ACESFitted(color);
            }
        }
        else
        {
            color = Tonemap(color, 1.5);
        }
    }

    color = pow(color, vec3(1.0 / 2.2));

    float outAlpha = 1.0;
    vec3 bgCol = backgroundCol;

#ifdef OPT_TRANSPARENT_BACKGROUND
    outAlpha = alpha;
    float checkerSize = 10.0;
    float res = max(sign(mod(
        floor(gl_FragCoord.x / checkerSize) +
        floor(gl_FragCoord.y / checkerSize), 2.0
    )), 0.0);
    bgCol = mix(vec3(0.1), vec3(0.2), res);
#endif

#if defined(OPT_BACKGROUND) || defined(OPT_TRANSPARENT_BACKGROUND)
    outCol = vec4(mix(bgCol, color, alpha), outAlpha);
#else
    outCol = vec4(color, 1.0);
#endif
}

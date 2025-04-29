///////////////////////////////////////////////////////////////////////////////
// OpenGL Version
///////////////////////////////////////////////////////////////////////////////
#version 330

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
out vec4 color;
in vec2 TexCoords;

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Uniforms.glsl"
#include "Core/Globals.glsl"
#include "Intersection/Intersection.glsl"
#include "Core/Sampling.glsl"
#include "Materials/Envmap.glsl"
#include "Intersection/AnyHit.glsl"
#include "Intersection/ClosestHit.glsl"
#include "Materials/Disney.glsl"
#include "Materials/Lambert.glsl"
#include "Intersection/Pathtrace.glsl"

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
void main(void)
{
    InitRNG(gl_FragCoord.xy, 1);

    float r1 = 2.0 * rand();
    float r2 = 2.0 * rand();

    vec2 jitter;
    jitter.x = r1 < 1.0 ? sqrt(r1) - 1.0 : 1.0 - sqrt(2.0 - r1);
    jitter.y = r2 < 1.0 ? sqrt(r2) - 1.0 : 1.0 - sqrt(2.0 - r2);

    jitter /= (resolution * 0.5);
    vec2 d = (2.0 * TexCoords - 1.0) + jitter;

    float scale = tan(camera.fov * 0.5);
    d.y *= resolution.y / resolution.x * scale;
    d.x *= scale;
    vec3 rayDir = normalize(d.x * camera.right + d.y * camera.up + camera.forward);

    vec3 focalPoint = camera.focalDist * rayDir;
    float cam_r1 = rand() * TWO_PI;
    float cam_r2 = rand() * camera.aperture;
    vec3 randomAperturePos =
        (cos(cam_r1) * camera.right + sin(cam_r1) * camera.up) * sqrt(cam_r2);
    vec3 finalRayDir = normalize(focalPoint - randomAperturePos);

    Ray ray = Ray(camera.position + randomAperturePos, finalRayDir);

    vec4 pixelColor = PathTrace(ray);

    color = pixelColor;
}

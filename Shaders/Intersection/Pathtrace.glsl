///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param state
/// \param r
///
///////////////////////////////////////////////////////////////////////////////
void GetMaterial(inout State state, in Ray r)
{
    int index = state.matID * 8;
    Material mat;
    Medium medium;

    vec4 param1 = texelFetch(materialsTex, ivec2(index + 0, 0), 0);
    vec4 param2 = texelFetch(materialsTex, ivec2(index + 1, 0), 0);
    vec4 param3 = texelFetch(materialsTex, ivec2(index + 2, 0), 0);
    vec4 param4 = texelFetch(materialsTex, ivec2(index + 3, 0), 0);
    vec4 param5 = texelFetch(materialsTex, ivec2(index + 4, 0), 0);
    vec4 param6 = texelFetch(materialsTex, ivec2(index + 5, 0), 0);
    vec4 param7 = texelFetch(materialsTex, ivec2(index + 6, 0), 0);
    vec4 param8 = texelFetch(materialsTex, ivec2(index + 7, 0), 0);

    mat.baseColor          = param1.rgb;
    mat.anisotropic        = param1.w;

    mat.emission           = param2.rgb;

    mat.metallic           = param3.x;
    mat.roughness          = max(param3.y, 0.001);
    mat.subsurface         = param3.z;
    mat.specularTint       = param3.w;

    mat.sheen              = param4.x;
    mat.sheenTint          = param4.y;
    mat.clearcoat          = param4.z;
    mat.clearcoatRoughness = mix(0.1, 0.001, param4.w);

    mat.specTrans          = param5.x;
    mat.ior                = param5.y;
    mat.medium.type        = int(param5.z);
    mat.medium.density     = param5.w;

    mat.medium.color       = param6.rgb;
    mat.medium.anisotropy  = clamp(param6.w, -0.9, 0.9);

    ivec4 texIDs           = ivec4(param7);

    mat.opacity            = param8.x;
    mat.alphaMode          = int(param8.y);
    mat.alphaCutoff        = param8.z;

    if (texIDs.x >= 0)
    {
        vec4 col = texture(textureMapsArrayTex, vec3(state.texCoord, texIDs.x));
        mat.baseColor.rgb *= pow(col.rgb, vec3(2.2));
        mat.opacity *= col.a;
    }

    if (texIDs.y >= 0)
    {
        vec2 matRgh = texture(textureMapsArrayTex, vec3(state.texCoord, texIDs.y)).bg;
        mat.metallic = matRgh.x;
        mat.roughness = max(matRgh.y * matRgh.y, 0.001);
    }

    if (texIDs.z >= 0)
    {
        vec3 texNormal = texture(textureMapsArrayTex, vec3(state.texCoord, texIDs.z)).rgb;

#ifdef OPT_OPENGL_NORMALMAP
        texNormal.y = 1.0 - texNormal.y;
#endif
        texNormal = normalize(texNormal * 2.0 - 1.0);

        vec3 origNormal = state.normal;
        state.normal = normalize(state.tangent * texNormal.x + state.bitangent * texNormal.y + state.normal * texNormal.z);
        state.ffnormal = dot(origNormal, r.direction) <= 0.0 ? state.normal : -state.normal;
    }

#ifdef OPT_ROUGHNESS_MOLLIFICATION
    if(state.depth > 0)
    {
        mat.roughness = max(mix(0.0, state.mat.roughness, roughnessMollificationAmt), mat.roughness);
    }
#endif

    if (texIDs.w >= 0)
    {
        mat.emission = pow(texture(textureMapsArrayTex, vec3(state.texCoord, texIDs.w)).rgb, vec3(2.2));
    }

    float aspect = sqrt(1.0 - mat.anisotropic * 0.9);
    mat.ax = max(0.001, mat.roughness / aspect);
    mat.ay = max(0.001, mat.roughness * aspect);

    state.mat = mat;
    state.eta = dot(r.direction, state.normal) < 0.0 ? (1.0 / mat.ior) : mat.ior;
}

#if defined(OPT_MEDIUM) && defined(OPT_VOL_MIS)
///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param r
///
/// \return
///
///////////////////////////////////////////////////////////////////////////////
vec3 EvalTransmittance(Ray r)
{
    LightSampleRec lightSample;
    State state;
    vec3 transmittance = vec3(1.0);

    for (int depth = 0; depth < maxDepth; depth++)
    {
        bool hit = ClosestHit(r, state, lightSample);

        if (!hit || state.isEmitter)
        {
            break;
        }

        GetMaterial(state, r);

        bool alphatest = (state.mat.alphaMode == ALPHA_MODE_MASK && state.mat.opacity < state.mat.alphaCutoff) || (state.mat.alphaMode == ALPHA_MODE_BLEND && rand() > state.mat.opacity);
        bool refractive = (1.0 - state.mat.metallic) * state.mat.specTrans > 0.0;

        if(hit && !(alphatest || refractive))
        {
            return (vec3(0.0));
        }

        if (dot(r.direction, state.normal) > 0 && state.mat.medium.type != MEDIUM_NONE)
        {
            vec3 color = state.mat.medium.type == MEDIUM_ABSORB ? vec3(1.0) - state.mat.medium.color : vec3(1.0);
            transmittance *= exp(-color * state.mat.medium.density * state.hitDist);
        }

        r.origin = state.fhp + r.direction * EPS;
    }

    return (transmittance);
}
#endif

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param r
/// \param state
/// \param isSurface
///
/// \return
///
///////////////////////////////////////////////////////////////////////////////
vec3 DirectLight(in Ray r, in State state, bool isSurface)
{
    vec3 Ld = vec3(0.0);
    vec3 Li = vec3(0.0);
    vec3 scatterPos = state.fhp + state.normal * EPS;

    ScatterSampleRec scatterSample;

#ifdef OPT_ENVMAP
#ifndef OPT_UNIFORM_LIGHT
    {
        vec3 color;
        vec4 dirPdf = SampleEnvMap(Li);
        vec3 lightDir = dirPdf.xyz;
        float lightPdf = dirPdf.w;

        Ray shadowRay = Ray(scatterPos, lightDir);

#if defined(OPT_MEDIUM) && defined(OPT_VOL_MIS)

        Li *= EvalTransmittance(shadowRay);

        if (isSurface)
        {
            scatterSample.f = DisneyEval(state, -r.direction, state.ffnormal, lightDir, scatterSample.pdf);
        }
        else
        {
            float p = PhaseHG(dot(-r.direction, lightDir), state.medium.anisotropy);
            scatterSample.f = vec3(p);
            scatterSample.pdf = p;
        }

        if (scatterSample.pdf > 0.0)
        {
            float misWeight = PowerHeuristic(lightPdf, scatterSample.pdf);
            if (misWeight > 0.0)
            {
                Ld += misWeight * Li * scatterSample.f * envMapIntensity / lightPdf;
            }
        }
#else

        bool inShadow = AnyHit(shadowRay, INF - EPS);

        if (!inShadow)
        {
            scatterSample.f = DisneyEval(state, -r.direction, state.ffnormal, lightDir, scatterSample.pdf);

            if (scatterSample.pdf > 0.0)
            {
                float misWeight = PowerHeuristic(lightPdf, scatterSample.pdf);
                if (misWeight > 0.0)
                {
                    Ld += misWeight * Li * scatterSample.f * envMapIntensity / lightPdf;
                }
            }
        }
#endif
    }
#endif
#endif

#ifdef OPT_LIGHTS
    {
        LightSampleRec lightSample;
        Light light;

        int index = int(rand() * float(numOfLights)) * 5;

        vec3 position = texelFetch(lightsTex, ivec2(index + 0, 0), 0).xyz;
        vec3 emission = texelFetch(lightsTex, ivec2(index + 1, 0), 0).xyz;
        vec3 u        = texelFetch(lightsTex, ivec2(index + 2, 0), 0).xyz;
        vec3 v        = texelFetch(lightsTex, ivec2(index + 3, 0), 0).xyz;
        vec3 params   = texelFetch(lightsTex, ivec2(index + 4, 0), 0).xyz;
        float radius  = params.x;
        float area    = params.y;
        float type    = params.z;

        light = Light(position, emission, u, v, radius, area, type);
        SampleOneLight(light, scatterPos, lightSample);
        Li = lightSample.emission;

        if (dot(lightSample.direction, lightSample.normal) < 0.0)
        {
            Ray shadowRay = Ray(scatterPos, lightSample.direction);

#if defined(OPT_MEDIUM) && defined(OPT_VOL_MIS)
            Li *= EvalTransmittance(shadowRay);

            if (isSurface)
            {
                scatterSample.f = DisneyEval(state, -r.direction, state.ffnormal, lightSample.direction, scatterSample.pdf);
            }
            else
            {
                float p = PhaseHG(dot(-r.direction, lightSample.direction), state.medium.anisotropy);
                scatterSample.f = vec3(p);
                scatterSample.pdf = p;
            }

            float misWeight = 1.0;
            if(light.area > 0.0)
            {
                misWeight = PowerHeuristic(lightSample.pdf, scatterSample.pdf);
            }

            if (scatterSample.pdf > 0.0)
            {
                Ld += misWeight * scatterSample.f * Li / lightSample.pdf;
            }
#else

            bool inShadow = AnyHit(shadowRay, lightSample.dist - EPS);

            if (!inShadow)
            {
                scatterSample.f = DisneyEval(state, -r.direction, state.ffnormal, lightSample.direction, scatterSample.pdf);

                float misWeight = 1.0;
                if(light.area > 0.0)
                {
                    misWeight = PowerHeuristic(lightSample.pdf, scatterSample.pdf);
                }

                if (scatterSample.pdf > 0.0)
                {
                    Ld += misWeight * Li * scatterSample.f / lightSample.pdf;
                }
            }
#endif
        }
    }
#endif

    return (Ld);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param r
///
/// \return
///
///////////////////////////////////////////////////////////////////////////////
vec4 PathTrace(Ray r)
{
    vec3 radiance = vec3(0.0);
    vec3 throughput = vec3(1.0);
    State state;
    LightSampleRec lightSample;
    ScatterSampleRec scatterSample;

    float alpha = 1.0;

    bool inMedium = false;
    bool mediumSampled = false;
    bool surfaceScatter = false;

    for (state.depth = 0;; state.depth++)
    {
        bool hit = ClosestHit(r, state, lightSample);

        if (!hit)
        {
#if defined(OPT_BACKGROUND) || defined(OPT_TRANSPARENT_BACKGROUND)
            if (state.depth == 0)
                alpha = 0.0;
#endif

#ifdef OPT_HIDE_EMITTERS
            if(state.depth > 0)
#endif
            {
#ifdef OPT_UNIFORM_LIGHT
                radiance += uniformLightCol * throughput;
#else
#ifdef OPT_ENVMAP
                vec4 envMapColPdf = EvalEnvMap(r);

                float misWeight = 1.0;

                if (state.depth > 0)
                {
                    misWeight = PowerHeuristic(scatterSample.pdf, envMapColPdf.w);
                }

#if defined(OPT_MEDIUM) && !defined(OPT_VOL_MIS)
                if(!surfaceScatter)
                {
                    misWeight = 1.0f;
                }
#endif

                if(misWeight > 0)
                {
                    radiance += misWeight * envMapColPdf.rgb * throughput * envMapIntensity;
                }
#endif
#endif
             }
             break;
        }

        GetMaterial(state, r);

        radiance += state.mat.emission * throughput;

#ifdef OPT_LIGHTS

        if (state.isEmitter)
        {
            float misWeight = 1.0;

            if (state.depth > 0)
            {
                misWeight = PowerHeuristic(scatterSample.pdf, lightSample.pdf);
            }

#if defined(OPT_MEDIUM) && !defined(OPT_VOL_MIS)
            if(!surfaceScatter)
            {
                misWeight = 1.0f;
            }
#endif

            radiance += misWeight * lightSample.emission * throughput;

            break;
        }
#endif

        if(state.depth == maxDepth)
            break;

#ifdef OPT_MEDIUM

        mediumSampled = false;
        surfaceScatter = false;

        if(inMedium)
        {
            if(state.medium.type == MEDIUM_ABSORB)
            {
                throughput *= exp(-(1.0 - state.medium.color) * state.hitDist * state.medium.density);
            }
            else if(state.medium.type == MEDIUM_EMISSIVE)
            {
                radiance += state.medium.color * state.hitDist * state.medium.density * throughput;
            }
            else
            {

                float scatterDist = min(-log(rand()) / state.medium.density, state.hitDist);
                mediumSampled = scatterDist < state.hitDist;

                if (mediumSampled)
                {
                    throughput *= state.medium.color;

                    r.origin += r.direction * scatterDist;
                    state.fhp = r.origin;

                    radiance += DirectLight(r, state, false) * throughput;

                    vec3 scatterDir = SampleHG(-r.direction, state.medium.anisotropy, rand(), rand());
                    scatterSample.pdf = PhaseHG(dot(-r.direction, scatterDir), state.medium.anisotropy);
                    r.direction = scatterDir;
                }
            }
        }

        if (!mediumSampled)
        {
#endif
#ifdef OPT_ALPHA_TEST

            if ((state.mat.alphaMode == ALPHA_MODE_MASK && state.mat.opacity < state.mat.alphaCutoff) ||
                (state.mat.alphaMode == ALPHA_MODE_BLEND && rand() > state.mat.opacity))
            {
                scatterSample.L = r.direction;
                state.depth--;
            }
            else
#endif
            {
                surfaceScatter = true;

                radiance += DirectLight(r, state, true) * throughput;

                scatterSample.f = DisneySample(state, -r.direction, state.ffnormal, scatterSample.L, scatterSample.pdf);
                if (scatterSample.pdf > 0.0)
                {
                    throughput *= scatterSample.f / scatterSample.pdf;
                }
                else
                {
                    break;
                }
            }

            r.direction = scatterSample.L;
            r.origin = state.fhp + r.direction * EPS;

#ifdef OPT_MEDIUM

            if (dot(r.direction, state.normal) < 0 && state.mat.medium.type != MEDIUM_NONE)
            {
                inMedium = true;

                state.medium = state.mat.medium;
            }

            else if (state.mat.medium.type != MEDIUM_NONE)
            {
                inMedium = false;
            }
        }
#endif

#ifdef OPT_RR

        if (state.depth >= OPT_RR_DEPTH)
        {
            float q = min(max(throughput.x, max(throughput.y, throughput.z)) + 0.001, 0.95);
            if (rand() > q)
            {
                break;
            }
            throughput /= q;
        }
#endif

    }

    return (vec4(radiance, alpha));
}

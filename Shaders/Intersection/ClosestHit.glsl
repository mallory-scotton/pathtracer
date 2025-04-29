///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param r
/// \param state
/// \param lightSample
///
/// \return
///
///////////////////////////////////////////////////////////////////////////////
bool ClosestHit(Ray r, inout State state, inout LightSampleRec lightSample)
{
    float t = INF;
    float d;

#ifdef OPT_LIGHTS
#ifdef OPT_HIDE_EMITTERS
if(state.depth > 0)
#endif
    for (int i = 0; i < numOfLights; i++)
    {
        vec3 position = texelFetch(lightsTex, ivec2(i * 5 + 0, 0), 0).xyz;
        vec3 emission = texelFetch(lightsTex, ivec2(i * 5 + 1, 0), 0).xyz;
        vec3 u        = texelFetch(lightsTex, ivec2(i * 5 + 2, 0), 0).xyz;
        vec3 v        = texelFetch(lightsTex, ivec2(i * 5 + 3, 0), 0).xyz;
        vec3 params   = texelFetch(lightsTex, ivec2(i * 5 + 4, 0), 0).xyz;
        float radius  = params.x;
        float area    = params.y;
        float type    = params.z;

        if (type == QUAD_LIGHT)
        {
            vec3 normal = normalize(cross(u, v));
            if (dot(normal, r.direction) > 0.)
            {
                continue;
            }
            vec4 plane = vec4(normal, dot(normal, position));
            u *= 1.0f / dot(u, u);
            v *= 1.0f / dot(v, v);

            d = RectIntersect(position, u, v, plane, r);
            if (d < 0.)
            {
                d = INF;
            }
            if (d < t)
            {
                t = d;
                float cosTheta = dot(-r.direction, normal);
                lightSample.pdf = (t * t) / (area * cosTheta);
                lightSample.emission = emission;
                state.isEmitter = true;
            }
        }

        if (type == SPHERE_LIGHT)
        {
            d = SphereIntersect(radius, position, r);
            if (d < 0.)
            {
                d = INF;
            }
            if (d < t)
            {
                t = d;
                vec3 hitPt = r.origin + t * r.direction;
                float cosTheta = dot(-r.direction, normalize(hitPt - position));
                lightSample.pdf = (t * t) / (area * cosTheta * 0.5);
                lightSample.emission = emission;
                state.isEmitter = true;
            }
        }
    }
#endif

    int stack[64];
    int ptr = 0;
    stack[ptr++] = -1;

    int index = topBVHIndex;
    float leftHit = 0.0;
    float rightHit = 0.0;

    int currMatID = 0;
    bool BLAS = false;

    ivec3 triID = ivec3(-1);
    mat4 transMat;
    mat4 transform;
    vec3 bary;
    vec4 vert0, vert1, vert2;

    Ray rTrans;
    rTrans.origin = r.origin;
    rTrans.direction = r.direction;

    while (index != -1)
    {
        ivec3 LRLeaf = ivec3(texelFetch(BVH, index * 3 + 2).xyz);

        int leftIndex  = int(LRLeaf.x);
        int rightIndex = int(LRLeaf.y);
        int leaf       = int(LRLeaf.z);

        if (leaf > 0)
        {
            for (int i = 0; i < rightIndex; i++)
            {
                ivec3 vertIndices = ivec3(texelFetch(vertexIndicesTex, leftIndex + i).xyz);

                vec4 v0 = texelFetch(verticesTex, vertIndices.x);
                vec4 v1 = texelFetch(verticesTex, vertIndices.y);
                vec4 v2 = texelFetch(verticesTex, vertIndices.z);

                vec3 e0 = v1.xyz - v0.xyz;
                vec3 e1 = v2.xyz - v0.xyz;
                vec3 pv = cross(rTrans.direction, e1);
                float det = dot(e0, pv);

                vec3 tv = rTrans.origin - v0.xyz;
                vec3 qv = cross(tv, e0);

                vec4 uvt;
                uvt.x = dot(tv, pv);
                uvt.y = dot(rTrans.direction, qv);
                uvt.z = dot(e1, qv);
                uvt.xyz = uvt.xyz / det;
                uvt.w = 1.0 - uvt.x - uvt.y;

                if (all(greaterThanEqual(uvt, vec4(0.0))) && uvt.z < t)
                {
                    t = uvt.z;
                    triID = vertIndices;
                    state.matID = currMatID;
                    bary = uvt.wxy;
                    vert0 = v0, vert1 = v1, vert2 = v2;
                    transform = transMat;
                }
            }
        }
        else if (leaf < 0)
        {
            vec4 r1 = texelFetch(transformsTex, ivec2((-leaf - 1) * 4 + 0, 0), 0).xyzw;
            vec4 r2 = texelFetch(transformsTex, ivec2((-leaf - 1) * 4 + 1, 0), 0).xyzw;
            vec4 r3 = texelFetch(transformsTex, ivec2((-leaf - 1) * 4 + 2, 0), 0).xyzw;
            vec4 r4 = texelFetch(transformsTex, ivec2((-leaf - 1) * 4 + 3, 0), 0).xyzw;

            transMat = mat4(r1, r2, r3, r4);

            rTrans.origin    = vec3(inverse(transMat) * vec4(r.origin, 1.0));
            rTrans.direction = vec3(inverse(transMat) * vec4(r.direction, 0.0));

            stack[ptr++] = -1;
            index = leftIndex;
            BLAS = true;
            currMatID = rightIndex;
            continue;
        }
        else
        {
            leftHit  = AABBIntersect(
                texelFetch(BVH, leftIndex  * 3 + 0).xyz,
                texelFetch(BVH, leftIndex  * 3 + 1).xyz, rTrans
            );
            rightHit = AABBIntersect(
                texelFetch(BVH, rightIndex * 3 + 0).xyz,
                texelFetch(BVH, rightIndex * 3 + 1).xyz, rTrans
            );

            if (leftHit > 0.0 && rightHit > 0.0)
            {
                int deferred = -1;
                if (leftHit > rightHit)
                {
                    index = rightIndex;
                    deferred = leftIndex;
                }
                else
                {
                    index = leftIndex;
                    deferred = rightIndex;
                }

                stack[ptr++] = deferred;
                continue;
            }
            else if (leftHit > 0.)
            {
                index = leftIndex;
                continue;
            }
            else if (rightHit > 0.)
            {
                index = rightIndex;
                continue;
            }
        }
        index = stack[--ptr];

        if (BLAS && index == -1)
        {
            BLAS = false;

            index = stack[--ptr];

            rTrans.origin = r.origin;
            rTrans.direction = r.direction;
        }
    }

    if (t == INF)
    {
        return (false);
    }

    state.hitDist = t;
    state.fhp = r.origin + r.direction * t;

    if (triID.x != -1)
    {
        state.isEmitter = false;

        vec4 n0 = texelFetch(normalsTex, triID.x);
        vec4 n1 = texelFetch(normalsTex, triID.y);
        vec4 n2 = texelFetch(normalsTex, triID.z);

        vec2 t0 = vec2(vert0.w, n0.w);
        vec2 t1 = vec2(vert1.w, n1.w);
        vec2 t2 = vec2(vert2.w, n2.w);

        state.texCoord = t0 * bary.x + t1 * bary.y + t2 * bary.z;
        vec3 normal = normalize(n0.xyz * bary.x + n1.xyz * bary.y + n2.xyz * bary.z);

        state.normal = normalize(transpose(inverse(mat3(transform))) * normal);
        state.ffnormal = dot(state.normal, r.direction) <= 0.0 ? state.normal : -state.normal;

        vec3 deltaPos1 = vert1.xyz - vert0.xyz;
        vec3 deltaPos2 = vert2.xyz - vert0.xyz;

        vec2 deltaUV1 = t1 - t0;
        vec2 deltaUV2 = t2 - t0;

        float invdet = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

        state.tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * invdet;
        state.bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * invdet;

        state.tangent = normalize(mat3(transform) * state.tangent);
        state.bitangent = normalize(mat3(transform) * state.bitangent);
    }

    return (true);
}

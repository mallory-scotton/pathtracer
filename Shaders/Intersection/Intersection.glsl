///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param rad
/// \param pos
/// \param r
///
/// \return
///
///////////////////////////////////////////////////////////////////////////////
float SphereIntersect(float rad, vec3 pos, Ray r)
{
    vec3 op = pos - r.origin;
    float eps = 0.001;
    float b = dot(op, r.direction);
    float det = b * b - dot(op, op) + rad * rad;
    if (det < 0.0)
    {
        return (INF);
    }

    det = sqrt(det);
    float t1 = b - det;
    if (t1 > eps)
    {
        return (t1);
    }

    float t2 = b + det;
    if (t2 > eps)
    {
        return (t2);
    }

    return (INF);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param pos
/// \param u
/// \param v
/// \param plane
/// \param r
///
/// \return
///
///////////////////////////////////////////////////////////////////////////////
float RectIntersect(in vec3 pos, in vec3 u, in vec3 v, in vec4 plane, in Ray r)
{
    vec3 n = vec3(plane);
    float dt = dot(r.direction, n);
    float t = (plane.w - dot(n, r.origin)) / dt;

    if (t > EPS)
    {
        vec3 p = r.origin + r.direction * t;
        vec3 vi = p - pos;
        float a1 = dot(u, vi);
        if (a1 >= 0.0 && a1 <= 1.0)
        {
            float a2 = dot(v, vi);
            if (a2 >= 0.0 && a2 <= 1.0)
            {
                return (t);
            }
        }
    }

    return (INF);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
/// \param minCorner
/// \param maxCorner
/// \param r
///
/// \return
///
///////////////////////////////////////////////////////////////////////////////
float AABBIntersect(vec3 minCorner, vec3 maxCorner, Ray r)
{
    vec3 invDir = 1.0 / r.direction;

    vec3 f = (maxCorner - r.origin) * invDir;
    vec3 n = (minCorner - r.origin) * invDir;

    vec3 tmax = max(f, n);
    vec3 tmin = min(f, n);

    float t1 = min(tmax.x, min(tmax.y, tmax.z));
    float t0 = max(tmin.x, max(tmin.y, tmin.z));

    return ((t1 >= t0) ? (t0 > 0.f ? t0 : t1) : -1.0);
}

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Objects/MobiusStrip.hpp"
#include "Maths/Constants.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray::Objects
///////////////////////////////////////////////////////////////////////////////
namespace Ray::Objects
{

///////////////////////////////////////////////////////////////////////////////
const MobiusStrip::ConstructorType& MobiusStrip::Constructor =
    [](const Optional<LibConfig::Setting>& config) -> UniquePtr<IObject>
{
    float radius = 1.0f;
    float halfWidth = 0.2f;
    int segments = 128;

    if (config.has_value())
    {
        config->Value("radius", radius);
        config->Value("halfWidth", halfWidth);
        config->Value("segments", segments);
    }
    
    return (std::make_unique<Objects::MobiusStrip>(
        radius, halfWidth, segments
    ));
};

///////////////////////////////////////////////////////////////////////////////
MobiusStrip::MobiusStrip(float radius, float halfWidth, int segments)
    : APrimitiveObject("mobius_strip")
    , m_radius(radius)
    , m_halfWidth(halfWidth)
    , m_segments(segments)
{
    if (m_radius <= 0.0f)
    {
        m_radius = 1.0f;
    }

    if (m_halfWidth <= 0.0f)
    {
        m_halfWidth = 0.1f;
    }

    if (m_segments < 3)
    {
        m_segments = 3;
    }

    m_hash = m_name +
        std::to_string(m_radius) +
        std::to_string(m_halfWidth) +
        std::to_string(m_segments);

    GenerateGeometry();
}

///////////////////////////////////////////////////////////////////////////////
void MobiusStrip::AddTriangle(
    const Vec4f& v1,
    const Vec4f& v2,
    const Vec4f& v3
)
{
    Vec4f p1 = v1; p1.w = 1.0f;
    Vec4f p2 = v2; p2.w = 1.0f;
    Vec4f p3 = v3; p3.w = 1.0f;

    Vec4f edge1 = p2 - p1; edge1.w = 0.0f;
    Vec4f edge2 = p3 - p1; edge2.w = 0.0f;

    Vec4f normal = edge1.Cross(edge2);
    normal.w = 0.0f;
    normal = normal.Normalize();

    m_vertices.push_back(p1);
    m_vertices.push_back(p2);
    m_vertices.push_back(p3);

    m_normals.push_back(normal);
    m_normals.push_back(normal);
    m_normals.push_back(normal);
}

///////////////////////////////////////////////////////////////////////////////
void MobiusStrip::GenerateGeometry(void)
{
    m_vertices.clear();
    m_normals.clear();

    if (m_segments <= 0 || m_radius <= 0.0f || m_halfWidth <= 0.0f)
    {
        return;
    }

    Vector<Vec4f> loop_points;
    loop_points.reserve(static_cast<size_t>(m_segments + 1) * 2);

    for (int i = 0; i <= m_segments; ++i)
    {
        float u_angle = (static_cast<float>(i) /
            static_cast<float>(m_segments)) * 2.0f * PI;

        float cos_u = cosf(u_angle);
        float sin_u = sinf(u_angle);
        float u_half_angle = u_angle / 2.0f;
        float cos_u_half = cosf(u_half_angle);
        float sin_u_half = sinf(u_half_angle);

        float x1 = (m_radius - m_halfWidth * cos_u_half) * cos_u;
        float y1 = (m_radius - m_halfWidth * cos_u_half) * sin_u;
        float z1 = -m_halfWidth * sin_u_half;
        loop_points.emplace_back(x1, y1, z1, 1.0f);

        float x2 = (m_radius + m_halfWidth * cos_u_half) * cos_u;
        float y2 = (m_radius + m_halfWidth * cos_u_half) * sin_u;
        float z2 = m_halfWidth * sin_u_half;
        loop_points.emplace_back(x2, y2, z2, 1.0f);
    }

    for (int i = 0; i < m_segments; ++i)
    {
        const Vec4f& p0 = loop_points[static_cast<size_t>(i) * 2];
        const Vec4f& p1 = loop_points[static_cast<size_t>(i) * 2 + 1];
        const Vec4f& p2_next_u_neg_w =
            loop_points[static_cast<size_t>(i + 1) * 2];
        const Vec4f& p3_next_u_pos_w =
            loop_points[static_cast<size_t>(i + 1) * 2 + 1];

        AddTriangle(p0, p1, p3_next_u_pos_w);
        AddTriangle(p0, p3_next_u_pos_w, p2_next_u_neg_w);
    }
}

} // namespace Ray::Objects

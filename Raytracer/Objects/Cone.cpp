///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Objects/Cone.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray::Objects
///////////////////////////////////////////////////////////////////////////////
namespace Ray::Objects
{

///////////////////////////////////////////////////////////////////////////////
const Cone::ConstructorType& Cone::Constructor =
    [](const Optional<LibConfig::Setting>& config) -> UniquePtr<IObject>
{
    float radius = 0.5f;
    float height = 1.0f;
    int segments = 32;

    if (config.has_value())
    {
        config->Value("radius", radius);
        config->Value("height", height);
        config->Value("segments", segments);
    }

    return (std::make_unique<Objects::Cone>(radius, height, segments));
};

///////////////////////////////////////////////////////////////////////////////
Cone::Cone(float radius, float height, int segments)
    : APrimitiveObject("cone")
    , m_radius(radius)
    , m_height(height)
    , m_segments(segments)
{
    if (m_segments < 3)
    {
        m_segments = 3;
    }

    m_hash = m_name +
        std::to_string(m_radius) +
        std::to_string(m_height) +
        std::to_string(m_segments);

    GenerateGeometry();
}

///////////////////////////////////////////////////////////////////////////////
void Cone::GenerateGeometry(void)
{
    m_vertices.clear();
    m_normals.clear();

    const float halfHeight = m_height / 2.0f;
    const float angleStep = 2.0f * static_cast<float>(M_PI) / m_segments;

    Vec4f apex(0.0f, halfHeight, 0.0f, 1.0f);

    Vec4f baseCenter(0.0f, -halfHeight, 0.0f, 1.0f);
    Vec4f baseNormal(0.0f, -1.0f, 0.0f, 0.0f);

    const float slantHeight = sqrtf(m_radius * m_radius + m_height * m_height);
    const float normScaleY = (slantHeight > 1e-6f) ?
        (m_radius / slantHeight) : 1.0f;
    const float normScaleXZ = (slantHeight > 1e-6f) ?
        (m_height / slantHeight) : 0.0f;

    for (int i = 0; i < m_segments; ++i)
    {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        Vec4f v1_base(
            m_radius * cos(angle1), -halfHeight,
            m_radius * sin(angle1), 1.0f
        );
        Vec4f v2_base(
            m_radius * cos(angle2), -halfHeight,
            m_radius * sin(angle2), 1.0f
        );

        m_vertices.push_back(baseCenter);
        m_normals.push_back(baseNormal);
        m_vertices.push_back(v1_base);
        m_normals.push_back(baseNormal);
        m_vertices.push_back(v2_base);
        m_normals.push_back(baseNormal);

        Vec4f n1_side(
            cos(angle1) * normScaleXZ, normScaleY,
            sin(angle1) * normScaleXZ, 0.0f
        );

        Vec4f n2_side(
            cos(angle2) * normScaleXZ,normScaleY,
            sin(angle2) * normScaleXZ, 0.0f
        );

        Vec4f n_apex_segment = n1_side + n2_side;
        n_apex_segment.w = 0.0f;

        float n_apex_len_sq =
            n_apex_segment.x * n_apex_segment.x +
            n_apex_segment.y * n_apex_segment.y +
            n_apex_segment.z * n_apex_segment.z;
        if (n_apex_len_sq > 1e-9f)
        {
            float inv_len = 1.0f / sqrtf(n_apex_len_sq);
            n_apex_segment.x *= inv_len;
            n_apex_segment.y *= inv_len;
            n_apex_segment.z *= inv_len;
        }
        else
        {
            n_apex_segment = Vec4f(0.0f, 1.0f, 0.0f, 0.0f);
        }


        m_vertices.push_back(apex);
        m_normals.push_back(n_apex_segment);
        m_vertices.push_back(v2_base);
        m_normals.push_back(n2_side);
        m_vertices.push_back(v1_base);
        m_normals.push_back(n1_side);
    }
}

} // namespace Ray::Objects

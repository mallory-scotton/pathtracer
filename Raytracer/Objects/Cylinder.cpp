///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Objects/Cylinder.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray::Objects
///////////////////////////////////////////////////////////////////////////////
namespace Ray::Objects
{

///////////////////////////////////////////////////////////////////////////////
Cylinder::Cylinder(float radius, float height, bool capped, int segments)
    : APrimitiveObject("cylinder")
    , m_radius(radius)
    , m_height(height)
    , m_capped(capped)
    , m_segments(segments)
{
    if (m_segments < 3)
    {
        m_segments = 3;
    }

    GenerateGeometry();
}

///////////////////////////////////////////////////////////////////////////////
void Cylinder::GenerateGeometry(void)
{
    m_vertices.clear();
    m_normals.clear();

    const float halfHeight = m_height / 2.0f;
    const float angleStep = 2.0f * static_cast<float>(M_PI) / m_segments;

    if (m_capped)
    {
        Vec4f topCenter(0.0f, halfHeight, 0.0f, 1.0f);
        Vec4f topNormal(0.0f, 1.0f, 0.0f, 0.0f);
        for (int i = 0; i < m_segments; i++)
        {
            float angle1 = i * angleStep;
            float angle2 = (i + 1) * angleStep;

            Vec4f p1(
                m_radius * cos(angle1), halfHeight,
                m_radius * sin(angle1), 1.0f
            );
            Vec4f p2(
                m_radius * cos(angle2), halfHeight,
                m_radius * sin(angle2), 1.0f
            );

            m_vertices.push_back(topCenter);
            m_normals.push_back(topNormal);
            m_vertices.push_back(p1);
            m_normals.push_back(topNormal);
            m_vertices.push_back(p2);
            m_normals.push_back(topNormal);
        }

        Vec4f bottomCenter(0.0f, -halfHeight, 0.0f, 1.0f);
        Vec4f bottomNormal(0.0f, -1.0f, 0.0f, 0.0f);
        for (int i = 0; i < m_segments; i++)
        {
            float angle1 = i * angleStep;
            float angle2 = (i + 1) * angleStep;

            Vec4f p1(
                m_radius * cos(angle1), -halfHeight,
                m_radius * sin(angle1), 1.0f
            );
            Vec4f p2(
                m_radius * cos(angle2), -halfHeight,
                m_radius * sin(angle2), 1.0f
            );

            m_vertices.push_back(bottomCenter);
            m_normals.push_back(bottomNormal);
            m_vertices.push_back(p2);
            m_normals.push_back(bottomNormal);
            m_vertices.push_back(p1);
            m_normals.push_back(bottomNormal);
        }
    }

    for (int i = 0; i < m_segments; i++)
    {
        float angle1 = i * angleStep;
        float angle2 = ((i + 1) % m_segments) * angleStep;

        float x1 = m_radius * cos(angle1);
        float z1 = m_radius * sin(angle1);
        float x2 = m_radius * cos(angle2);
        float z2 = m_radius * sin(angle2);

        Vec4f v_tl(x1, halfHeight, z1, 1.0f);
        Vec4f v_tr(x2, halfHeight, z2, 1.0f);
        Vec4f v_bl(x1, -halfHeight, z1, 1.0f);
        Vec4f v_br(x2, -halfHeight, z2, 1.0f);

        Vec4f n1(cos(angle1), 0.0f, sin(angle1), 0.0f);
        Vec4f n2(cos(angle2), 0.0f, sin(angle2), 0.0f);

        m_vertices.push_back(v_bl);
        m_normals.push_back(n1);
        m_vertices.push_back(v_tr);
        m_normals.push_back(n2);
        m_vertices.push_back(v_tl);
        m_normals.push_back(n1);

        m_vertices.push_back(v_bl);
        m_normals.push_back(n1);
        m_vertices.push_back(v_br);
        m_normals.push_back(n2);
        m_vertices.push_back(v_tr);
        m_normals.push_back(n2);
    }
}

} // namespace Ray::Objects

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Objects/Fractal.hpp"
#include "Maths/Constants.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray::Objects
///////////////////////////////////////////////////////////////////////////////
namespace Ray::Objects
{

///////////////////////////////////////////////////////////////////////////////
Fractal::Fractal(int iterations, float size)
    : APrimitiveObject("fractal")
    , m_iterations(iterations)
    , m_size(size)
{
    if (m_iterations < 0)
    {
        m_iterations = 0;
    }

    if (m_iterations > 5)
    {
        m_iterations = 5;
    }

    GenerateGeometry();
}

///////////////////////////////////////////////////////////////////////////////
void Fractal::AddTriangle(const Vec4f& v1, const Vec4f& v2, const Vec4f& v3)
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
void Fractal::GenerateSierpinskiRecursive(
    const Vec4f& p1,
    const Vec4f& p2,
    const Vec4f& p3,
    const Vec4f& p4,
    int current_depth
)
{
    if (current_depth == m_iterations)
    {
        AddTriangle(p1, p2, p3);
        AddTriangle(p1, p4, p2);
        AddTriangle(p2, p4, p3);
        AddTriangle(p3, p4, p1);
        return;
    }

    Vec4f m12 = (p1 + p2) * 0.5f; m12.w = 1.0f;
    Vec4f m13 = (p1 + p3) * 0.5f; m13.w = 1.0f;
    Vec4f m14 = (p1 + p4) * 0.5f; m14.w = 1.0f;
    Vec4f m23 = (p2 + p3) * 0.5f; m23.w = 1.0f;
    Vec4f m24 = (p2 + p4) * 0.5f; m24.w = 1.0f;
    Vec4f m34 = (p3 + p4) * 0.5f; m34.w = 1.0f;

    GenerateSierpinskiRecursive(p1,  m12, m13, m14, current_depth + 1);
    GenerateSierpinskiRecursive(m12, p2,  m23, m24, current_depth + 1);
    GenerateSierpinskiRecursive(m13, m23, p3,  m34, current_depth + 1);
    GenerateSierpinskiRecursive(m14, m24, m34, p4,  current_depth + 1);
}

///////////////////////////////////////////////////////////////////////////////
void Fractal::GenerateGeometry(void)
{
    m_vertices.clear();
    m_normals.clear();

    if (m_iterations < 0)
    {
        return;
    }

    if (m_size <= 0.0f)
    {
        return;
    }

    float s = m_size / 2.0f;

    Vec4f v1( s,  s,  s, 1.0f);
    Vec4f v2( s, -s, -s, 1.0f);
    Vec4f v3(-s,  s, -s, 1.0f);
    Vec4f v4(-s, -s,  s, 1.0f);

    GenerateSierpinskiRecursive(v1, v2, v3, v4, 0);
}

} // namespace Ray::Objects

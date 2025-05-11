///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Objects/Plane.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray::Objects
///////////////////////////////////////////////////////////////////////////////
namespace Ray::Objects
{

///////////////////////////////////////////////////////////////////////////////
Plane::Plane(void)
    : APrimitiveObject("plane")
{
    GenerateGeometry();
}

///////////////////////////////////////////////////////////////////////////////
void Plane::GenerateGeometry(void)
{
    m_vertices.clear();
    m_normals.clear();

    Vec4f v0(-0.5f, 0.0f, -0.5f, 1.0f);
    Vec4f v1( 0.5f, 0.0f, -0.5f, 1.0f);
    Vec4f v2( 0.5f, 0.0f,  0.5f, 1.0f);
    Vec4f v3(-0.5f, 0.0f,  0.5f, 1.0f);

    Vec4f normal(0.0f, 1.0f, 0.0f, 0.0f);

    m_vertices.push_back(v0);
    m_normals.push_back(normal);
    m_vertices.push_back(v1);
    m_normals.push_back(normal);
    m_vertices.push_back(v2);
    m_normals.push_back(normal);

    m_vertices.push_back(v0);
    m_normals.push_back(normal);
    m_vertices.push_back(v2);
    m_normals.push_back(normal);
    m_vertices.push_back(v3);
    m_normals.push_back(normal);
}

} // namespace Ray::Objects

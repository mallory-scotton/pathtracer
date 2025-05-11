///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Objects/Cube.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray::Objects
///////////////////////////////////////////////////////////////////////////////
namespace Ray::Objects
{

///////////////////////////////////////////////////////////////////////////////
Cube::Cube(float size)
    : APrimitiveObject("cube")
    , m_size(size)
{
    GenerateGeometry();
}

///////////////////////////////////////////////////////////////////////////////
void Cube::GenerateGeometry(void)
{
    // Clear any existing geometry data
    m_vertices.clear();
    m_normals.clear();

    // Calculate half-size for convenience in defining vertex coordinates
    float s = m_size / 2.0f;

    // Define the 8 unique vertices of the cube
    // The w-component is 1.0f for vertices
    Vec4f v0(-s, -s,  s, 1.0f);
    Vec4f v1( s, -s,  s, 1.0f);
    Vec4f v2( s,  s,  s, 1.0f);
    Vec4f v3(-s,  s,  s, 1.0f);
    Vec4f v4(-s, -s, -s, 1.0f);
    Vec4f v5( s, -s, -s, 1.0f);
    Vec4f v6( s,  s, -s, 1.0f);
    Vec4f v7(-s,  s, -s, 1.0f);

    // Define the 6 face normals for the cube
    // The w-component is 0.0f for normals
    Vec4f frontNormal( 0.0f,  0.0f,  1.0f, 0.0f);
    Vec4f backNormal(  0.0f,  0.0f, -1.0f, 0.0f);
    Vec4f rightNormal( 1.0f,  0.0f,  0.0f, 0.0f);
    Vec4f leftNormal( -1.0f,  0.0f,  0.0f, 0.0f);
    Vec4f topNormal(   0.0f,  1.0f,  0.0f, 0.0f);
    Vec4f bottomNormal(0.0f, -1.0f,  0.0f, 0.0f);

    // Front face (+z normal)
    // Triangle 1: v0, v1, v2
    m_vertices.push_back(v0); m_normals.push_back(frontNormal);
    m_vertices.push_back(v1); m_normals.push_back(frontNormal);
    m_vertices.push_back(v2); m_normals.push_back(frontNormal);
    // Triangle 2: v0, v2, v3
    m_vertices.push_back(v0); m_normals.push_back(frontNormal);
    m_vertices.push_back(v2); m_normals.push_back(frontNormal);
    m_vertices.push_back(v3); m_normals.push_back(frontNormal);

    // Back face (-z normal)
    // Vertices (CCW from outside): v5, v4, v7, v6
    // Triangle 1: v5, v4, v7
    m_vertices.push_back(v5); m_normals.push_back(backNormal);
    m_vertices.push_back(v4); m_normals.push_back(backNormal);
    m_vertices.push_back(v7); m_normals.push_back(backNormal);
    // Triangle 2: v5, v7, v6
    m_vertices.push_back(v5); m_normals.push_back(backNormal);
    m_vertices.push_back(v7); m_normals.push_back(backNormal);
    m_vertices.push_back(v6); m_normals.push_back(backNormal);

    // Right face (+x normal)
    // Vertices (CCW from outside): v1, v5, v6, v2
    // Triangle 1: v1, v5, v6
    m_vertices.push_back(v1); m_normals.push_back(rightNormal);
    m_vertices.push_back(v5); m_normals.push_back(rightNormal);
    m_vertices.push_back(v6); m_normals.push_back(rightNormal);
    // Triangle 2: v1, v6, v2
    m_vertices.push_back(v1); m_normals.push_back(rightNormal);
    m_vertices.push_back(v6); m_normals.push_back(rightNormal);
    m_vertices.push_back(v2); m_normals.push_back(rightNormal);

    // Left face (-x normal)
    // Vertices (CCW from outside): v4, v0, v3, v7
    // Triangle 1: v4, v0, v3
    m_vertices.push_back(v4); m_normals.push_back(leftNormal);
    m_vertices.push_back(v0); m_normals.push_back(leftNormal);
    m_vertices.push_back(v3); m_normals.push_back(leftNormal);
    // Triangle 2: v4, v3, v7
    m_vertices.push_back(v4); m_normals.push_back(leftNormal);
    m_vertices.push_back(v3); m_normals.push_back(leftNormal);
    m_vertices.push_back(v7); m_normals.push_back(leftNormal);

    // Top face (+y normal)
    // Vertices (CCW from outside): v3, v2, v6, v7
    // Triangle 1: v3, v2, v6
    m_vertices.push_back(v3); m_normals.push_back(topNormal);
    m_vertices.push_back(v2); m_normals.push_back(topNormal);
    m_vertices.push_back(v6); m_normals.push_back(topNormal);
    // Triangle 2: v3, v6, v7
    m_vertices.push_back(v3); m_normals.push_back(topNormal);
    m_vertices.push_back(v6); m_normals.push_back(topNormal);
    m_vertices.push_back(v7); m_normals.push_back(topNormal);

    // Bottom face (-y normal)
    // Vertices (CCW from outside): v4, v5, v1, v0
    // Triangle 1: v4, v5, v1
    m_vertices.push_back(v4); m_normals.push_back(bottomNormal);
    m_vertices.push_back(v5); m_normals.push_back(bottomNormal);
    m_vertices.push_back(v1); m_normals.push_back(bottomNormal);
    // Triangle 2: v4, v1, v0
    m_vertices.push_back(v4); m_normals.push_back(bottomNormal);
    m_vertices.push_back(v1); m_normals.push_back(bottomNormal);
    m_vertices.push_back(v0); m_normals.push_back(bottomNormal);
}

} // namespace Ray::Objects

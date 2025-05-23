///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Objects/InfinitePlane.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray::Objects
///////////////////////////////////////////////////////////////////////////////
namespace Ray::Objects
{

///////////////////////////////////////////////////////////////////////////////
const InfinitePlane::ConstructorType& InfinitePlane::Constructor =
    [](const Optional<LibConfig::Setting>& config) -> UniquePtr<IObject>
{
    RAY_UNUSED(config);
    return (std::make_unique<Objects::InfinitePlane>());
};

///////////////////////////////////////////////////////////////////////////////
InfinitePlane::InfinitePlane(void)
    : APrimitiveObject("infinite_plane")
{
    m_hash = m_name;
    GenerateGeometry();
}

///////////////////////////////////////////////////////////////////////////////
void InfinitePlane::GenerateGeometry(void)
{
    m_vertices.clear();
    m_normals.clear();

    const float limits = 1000000.0f;

    Vec4f v0(-limits, 0.0f, -limits, 1.0f);
    Vec4f v1( limits, 0.0f, -limits, 1.0f);
    Vec4f v2( limits, 0.0f,  limits, 1.0f);
    Vec4f v3(-limits, 0.0f,  limits, 1.0f);

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

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Objects/Prism.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray::Objects
///////////////////////////////////////////////////////////////////////////////
namespace Ray::Objects
{

///////////////////////////////////////////////////////////////////////////////
Prism::Prism(void)
    : APrimitiveObject("prism")
{
    GenerateGeometry();
}

///////////////////////////////////////////////////////////////////////////////
void Prism::GenerateGeometry(void)
{
    m_vertices.clear();
    m_normals.clear();
}

} // namespace Ray::Objects

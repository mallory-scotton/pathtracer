///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Factories/PrimitiveFactory.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
PrimitiveFactory::PrimitiveFactory(void)
{
    Register("cube", Objects::Cube::Constructor);
    Register("plane", Objects::Plane::Constructor);
    Register("sphere", Objects::Sphere::Constructor);
    Register("torus", Objects::Torus::Constructor);
    Register("infinite_plane", Objects::InfinitePlane::Constructor);
    Register("fractal", Objects::Fractal::Constructor);
    Register("cone", Objects::Cone::Constructor);
    Register("cylinder", Objects::Cylinder::Constructor);
    Register("mobius_strip", Objects::MobiusStrip::Constructor);
}

} // namespace Ray

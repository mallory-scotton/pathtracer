///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Acceleration/BoundingBox.hpp"
#include <limits>

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
BoundingBox::BoundingBox(void)
    : min(Vec3f(std::numeric_limits<float>::max()))
    , max(Vec3f(-std::numeric_limits<float>::max()))
{}

///////////////////////////////////////////////////////////////////////////////
BoundingBox::BoundingBox(const Vec3f& point)
    : min(point)
    , max(point)
{}

///////////////////////////////////////////////////////////////////////////////
BoundingBox::BoundingBox(const Vec3f& point1, const Vec3f& point2)
    : min(Vec3f::Min(point1, point2))
    , max(Vec3f::Max(point1, point2))
{}

///////////////////////////////////////////////////////////////////////////////
Vec3f& BoundingBox::operator[](Uint64 index)
{
    RAY_ASSERT(index < 2, RAY_ERROR_OVERFLOW);
    return (*(&min + index));
}

///////////////////////////////////////////////////////////////////////////////
const Vec3f& BoundingBox::operator[](Uint64 index) const
{
    RAY_ASSERT(index < 2, RAY_ERROR_OVERFLOW);
    return (*(&min + index));
}

///////////////////////////////////////////////////////////////////////////////
Vec3f BoundingBox::Center(void) const
{
    return ((max + min) * .5f);
}

///////////////////////////////////////////////////////////////////////////////
Vec3f BoundingBox::Extents(void) const
{
    return (max - min);
}

///////////////////////////////////////////////////////////////////////////////
bool BoundingBox::Contains(const Vec3f& point) const
{
    Vec3f radius = Extents() * 0.5f;
    return (
        std::abs(Center().x - point.x) <= radius.x &&
        std::abs(Center().y - point.y) <= radius.y &&
        std::abs(Center().z - point.z) <= radius.z
    );
}

///////////////////////////////////////////////////////////////////////////////
int BoundingBox::MaxDimension(void) const
{
    Vec3f ext = Extents();

    if (ext.x >= ext.y && ext.x >= ext.z)
    {
        return (0);
    }
    if (ext.y >= ext.x && ext.y >= ext.z)
    {
        return (1);
    }
    if (ext.z >= ext.x && ext.z >= ext.y)
    {
        return (2);
    }

    return (0);
}

///////////////////////////////////////////////////////////////////////////////
float BoundingBox::SurfaceArea(void) const
{
    Vec3f ext = Extents();
    return (2.f * (ext.x * ext.y + ext.x * ext.z + ext.y * ext.z));
}

///////////////////////////////////////////////////////////////////////////////
void BoundingBox::Grow(const Vec3f& point)
{
    min = Vec3f::Min(min, point);
    max = Vec3f::Max(max, point);
}

///////////////////////////////////////////////////////////////////////////////
void BoundingBox::Grow(const BoundingBox& box)
{
    min = Vec3f::Min(min, box.min);
    max = Vec3f::Max(max, box.max);
}

///////////////////////////////////////////////////////////////////////////////
BoundingBox BoundingBox::Union(const BoundingBox& a, const BoundingBox& b)
{
    return (BoundingBox(Vec3f::Min(a.min, b.min), Vec3f::Max(a.max, b.max)));
}

///////////////////////////////////////////////////////////////////////////////
BoundingBox BoundingBox::Intersection(
    const BoundingBox& a,
    const BoundingBox& b
)
{
    return (BoundingBox(Vec3f::Max(a.min, b.min), Vec3f::Min(a.max, b.max)));
}

///////////////////////////////////////////////////////////////////////////////
void BoundingBox::Intersects(
    const BoundingBox& a,
    const BoundingBox& b,
    BoundingBox& result
)
{
    result.min = Vec3f::Max(a.min, b.min);
    result.max = Vec3f::Min(a.max, b.max);
}

///////////////////////////////////////////////////////////////////////////////
bool BoundingBox::Contains(const BoundingBox& a, const BoundingBox& b)
{
    return (a.Contains(b.min) && a.Contains(b.max));
}

} // namespace Ray

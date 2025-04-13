///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Rays/BBox.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
BBox::BBox(void)
    : mMin(std::numeric_limits<float>::max())
    , mMax(std::numeric_limits<float>::max())
{}

///////////////////////////////////////////////////////////////////////////////
BBox::BBox(const Vec3f& point)
    : mMin(point)
    , mMax(point)
{}

///////////////////////////////////////////////////////////////////////////////
BBox::BBox(const Vec3f& point1, const Vec3f& point2)
    : mMin(Vec3f::Min(point1, point2))
    , mMax(Vec3f::Max(point1, point2))
{}

///////////////////////////////////////////////////////////////////////////////
const Vec3f& BBox::operator[](Uint64 index) const
{
    return (*(&mMin + index));
}

///////////////////////////////////////////////////////////////////////////////
BBox BBox::Union(const BBox& a, const BBox& b)
{
    return (BBox(
        Vec3f::Min(a.mMin, b.mMin),
        Vec3f::Max(a.mMax, b.mMax)
    ));
}

///////////////////////////////////////////////////////////////////////////////
BBox BBox::Intersection(
    const BBox& a,
    const BBox& b
)
{
    return (BBox(
        Vec3f::Max(a.mMin, b.mMin),
        Vec3f::Min(a.mMax, b.mMax)
    ));
}

///////////////////////////////////////////////////////////////////////////////
void BBox::Intersection(
    const BBox& a,
    const BBox& b,
    BBox& destination
)
{
    destination.mMin = Vec3f::Max(a.mMin, b.mMin);
    destination.mMax = Vec3f::Min(a.mMax, b.mMax);
}

///////////////////////////////////////////////////////////////////////////////
bool BBox::Intersects(const BBox& a, const BBox& b)
{
    static const float BOUNDING_BOX_INTERSECTION_EPS = 0.f;

    Vec3f aCenter = a.Center();
    Vec3f aRadius = a.Extents() * .5f;
    Vec3f bCenter = b.Center();
    Vec3f bRadius = b.Extents() * .5f;

    return (
        (std::abs(bCenter.x - aCenter.x) - (aRadius.x + bRadius.x))
            <= BOUNDING_BOX_INTERSECTION_EPS &&
        (std::abs(bCenter.y - aCenter.y) - (aRadius.y + bRadius.y))
            <= BOUNDING_BOX_INTERSECTION_EPS &&
        (std::abs(bCenter.z - aCenter.z) - (aRadius.z + bRadius.z))
            <= BOUNDING_BOX_INTERSECTION_EPS
    );
}

///////////////////////////////////////////////////////////////////////////////
bool BBox::Contains(const BBox& a, const BBox& b)
{
    return (a.Contains(b));
}

///////////////////////////////////////////////////////////////////////////////
Vec3f BBox::Center(void) const
{
    return ((mMax + mMin) * 0.5f);
}

///////////////////////////////////////////////////////////////////////////////
Vec3f BBox::Extents(void) const
{
    return (mMax - mMin);
}

///////////////////////////////////////////////////////////////////////////////
bool BBox::Contains(const Vec3f& point) const
{
    Vec3f radius = Extents() * .5f;
    Vec3f center = Center();

    return (
        std::abs(center.x - point.x) <= radius.x &&
        std::abs(center.y - point.y) <= radius.y &&
        std::abs(center.z - point.z) <= radius.y
    );
}

///////////////////////////////////////////////////////////////////////////////
bool BBox::Contains(const BBox& box) const
{
    return (Contains(box.mMin) && Contains(box.mMax));
}

///////////////////////////////////////////////////////////////////////////////
int BBox::MaxDimension(void) const
{
    Vec3f extents = Extents();

    if (extents.x >= extents.y && extents.x >= extents.z)
    {
        return (0);
    }

    if (extents.y >= extents.x && extents.y >= extents.z)
    {
        return (1);
    }

    if (extents.z >= extents.x && extents.z >= extents.y)
    {
        return (2);
    }

    return (0);
}

///////////////////////////////////////////////////////////////////////////////
float BBox::SurfaceArea(void) const
{
    Vec3f extents = Extents();

    return (2.f * (
        extents.x * extents.y +
        extents.x * extents.z +
        extents.y * extents.z
    ));
}

///////////////////////////////////////////////////////////////////////////////
void BBox::Grow(const Vec3f& point)
{
    mMin = Vec3f::Min(mMin, point);
    mMax = Vec3f::Max(mMax, point);
}

///////////////////////////////////////////////////////////////////////////////
void BBox::Grow(const BBox& box)
{
    mMin = Vec3f::Min(mMin, box.mMin);
    mMax = Vec3f::Max(mMax, box.mMax);
}

///////////////////////////////////////////////////////////////////////////////
Vec3f BBox::GetMin(void) const
{
    return (mMin);
}

///////////////////////////////////////////////////////////////////////////////
Vec3f BBox::GetMax(void) const
{
    return (mMax);
}

} // namespace Ray

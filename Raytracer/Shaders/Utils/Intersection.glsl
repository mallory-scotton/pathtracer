///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Types/Ray.glsl"
#include "Constants.glsl"

///////////////////////////////////////////////////////////////////////////////
/// \brief Calculates the intersection between a ray and a sphere
///
/// \param radius Radius of the sphere
/// \param position Center position of the sphere
/// \param ray Ray to test for intersection
///
/// \return Distance to intersection point, or INF if no intersection
///
///////////////////////////////////////////////////////////////////////////////
float SphereIntersect(float radius, vec3 position, Ray ray)
{
  // Vector from ray origin to sphere center
  vec3 originToCenter = position - ray.origin;
  float epsilon = 0.001;

  // Projection of originToCenter onto ray direction
  float projectionOnRay = dot(originToCenter, ray.direction);

  // Calculate discriminant for quadratic equation
  float discriminant =
    projectionOnRay * projectionOnRay -
    dot(originToCenter, originToCenter) +
    radius * radius;

  // No intersection if discriminant is negative
  if (discriminant < 0.0)
  {
    return (INF);
  }

  // Calculate both intersection distances
  discriminant = sqrt(discriminant);
  float nearDistance = projectionOnRay - discriminant;

  // Return nearest valid intersection
  if (nearDistance > epsilon)
  {
    return (nearDistance);
  }

  float farDistance = projectionOnRay + discriminant;
  if (farDistance > epsilon)
  {
    return (farDistance);
  }

  // No valid intersection found
  return (INF);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Calculates the intersection between a ray and a rectangle
///
/// \param position Origin position of the rectangle
/// \param u First edge direction vector (normalized, scaled to edge length)
/// \param v Second edge direction vector (normalized, scaled to edge length)
/// \param plane Plane equation coefficients (normal xyz, distance w)
/// \param ray Ray to test for intersection
///
/// \return Distance to intersection point, or INF if no intersection
///
///////////////////////////////////////////////////////////////////////////////
float RectIntersect(vec3 position, vec3 u, vec3 v, vec4 plane, Ray ray)
{
  // Extract normal vector from plane equation
  vec3 normal = vec3(plane);

  // Calculate denominator for plane intersection
  float denominator = dot(ray.direction, normal);

  // Calculate intersection distance with plane
  float intersectionDistance = (plane.w - dot(normal, ray.origin)) / denominator;

  // If intersection is not in front of ray origin, no valid intersection found
  if (intersectionDistance <= EPS)
  {
    return (INF);
  }

  // Calculate intersection point
  vec3 intersectionPoint = ray.origin + ray.direction * intersectionDistance;

  // Vector from rectangle origin to intersection point
  vec3 vectorToIntersection = intersectionPoint - position;

  // Project vector onto rectangle edges
  float uProjection = dot(u, vectorToIntersection);

  // Check if intersection is within rectangle bounds on u axis
  if (uProjection >= 0.0 && uProjection <= 1.0)
  {
    float vProjection = dot(v, vectorToIntersection);

    // Check if intersection is within rectangle bounds on v axis
    if (vProjection >= 0.0 && vProjection <= 1.0)
    {
      return (intersectionDistance);
    }
  }

  // No valid intersection found
  return (INF);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Calculates the intersection between a ray and an axis-aligned
/// bounding box
///
/// \param minCorner Minimum corner coordinates of the AABB
/// \param maxCorner Maximum corner coordinates of the AABB
/// \param ray Ray to test for intersection
///
/// \return  Distance to intersection point, or -1.0 if no intersection
///
///////////////////////////////////////////////////////////////////////////////
float AABBIntersect(vec3 minCorner, vec3 maxCorner, Ray ray)
{
  // Calculate inverse of ray direction to avoid divisions
  vec3 invDirection = 1.0 / ray.direction;

  // Calculate intersection distances for both sides of each axis
  vec3 distancesToMax = (maxCorner - ray.origin) * invDirection;
  vec3 distancesToMin = (minCorner - ray.origin) * invDirection;

  // Find furthest entry and closest exit points
  vec3 furthestEntryDistances = max(distancesToMin, distancesToMax);
  vec3 closestExitDistances = min(distancesToMin, distancesToMax);

  // Find closest exit point across all axes
  float exitDistance =
    min(furthestEntryDistances.x,
      min(furthestEntryDistances.y, furthestEntryDistances.z)
    );

  // Find furthest entry point across all axes
  float entryDistance =
    max(closestExitDistances.x,
      max(closestExitDistances.y, closestExitDistances.z)
    );

  // Check if there is a valid intersection
  if (exitDistance >= entryDistance)
  {
    // Return nearest valid intersection
    if (entryDistance > 0.0)
    {
      return (entryDistance);
    }
    return (exitDistance);
  }

  // No valid intersection found
  return (-1.0);
}

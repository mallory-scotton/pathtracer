///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#define TINYOBJLOADER_IMPLEMENTATION
#include "Resources/Mesh.hpp"
#include "Utils/Exception.hpp"
#include "Utils/Utils.hpp"
#include <tiny_obj_loader.h>

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
Mesh::Instance::Instance(
    const String& name,
    int meshID,
    const Mat4x4f& transform,
    int materialID
)
    : transform(transform)
    , name(name)
    , materialID(materialID)
    , meshID(meshID)
{}

///////////////////////////////////////////////////////////////////////////////
Mesh::Instance::Instance(const Map<String, Vector<String>>& props)
    : materialID(-1)
    , meshID(-1)
{
    Quaternionf rotation;

    bool hasProvidedAMatrix = false;
    Mat4x4f providedMatrix;
    Mat4x4f translate;
    Mat4x4f scale;
    Mat4x4f rotate;

    for (const auto& [key, values] : props)
    {
        Uint64 n = values.size();

        if (Utils::Equals(key, "name") && n == 1)
        {
            name = values[0];
        }
        else if (Utils::Equals(key, "position") && n == 3)
        {
            translate[3][0] = Utils::ToFloat(values[0]);
            translate[3][1] = Utils::ToFloat(values[1]);
            translate[3][2] = Utils::ToFloat(values[2]);
        }
        else if (Utils::Equals(key, "scale") && n == 3)
        {
            scale[0][0] = Utils::ToFloat(values[0]);
            scale[1][1] = Utils::ToFloat(values[1]);
            scale[2][2] = Utils::ToFloat(values[2]);
        }
        else if (Utils::Equals(key, "rotation") && n == 4)
        {
            rotation.x = Utils::ToFloat(values[0]);
            rotation.y = Utils::ToFloat(values[1]);
            rotation.z = Utils::ToFloat(values[2]);
            rotation.w = Utils::ToFloat(values[3]);

            rotate = Mat4x4f::QuaternionToMatrix(rotation);
        }
        else if (
            (Utils::Equals(key, "material") || Utils::Equals(key, "file")) &&
            n == 1
        )
        {
            // Nothing to do because this is handled by the Scene loader
        }
        else if (Utils::Equals(key, "matrix") && n == 16)
        {
            hasProvidedAMatrix = true;
            // TODO: Parse the matrix
        }
        else
        {
            RAY_WARN("\"" + key + "\" is not a valid Mesh attributes");
        }
    }

    if (hasProvidedAMatrix)
    {
        transform = providedMatrix;
    }
    else
    {
        transform = translate * rotate * scale;
    }
}

///////////////////////////////////////////////////////////////////////////////
Mesh::Mesh(void)
{
    // TODO: Add a split BVH instead of a simple BVH
    mBvh = std::make_shared<Bvh>(2.f, 64, false);
}

///////////////////////////////////////////////////////////////////////////////
Mesh::Mesh(const Path& filePath)
    : Mesh()
{
    if (!LoadFromFile(filePath))
    {
        throw Exception(
            "Failed to load Mesh files: \"" + filePath.string() + "\""
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Mesh::LoadFromFile(const Path& filePath)
{
    mName = filePath;

    tinyobj::attrib_t attrib;
    Vector<tinyobj::shape_t> shapes;
    Vector<tinyobj::material_t> materials;
    String err;

    if (!tinyobj::LoadObj(
        &attrib, &shapes, &materials, &err, filePath.c_str(), 0, true
    ))
    {
        RAY_WARN(RAY_ERROR_MISSING_MODEL << filePath);
        return (false);
    }

    for (const auto& shape : shapes)
    {
        Uint64 indexOffset = 0;

        for (Uint64 f = 0; f < shape.mesh.num_face_vertices.size(); f++)
        {
            for (Uint64 v = 0; v < 3; v++)
            {
                tinyobj::index_t idx = shape.mesh.indices[indexOffset + v];
                tinyobj::real_t vx =  attrib.vertices[3 * idx.vertex_index + 0];
                tinyobj::real_t vy =  attrib.vertices[3 * idx.vertex_index + 1];
                tinyobj::real_t vz =  attrib.vertices[3 * idx.vertex_index + 2];
                tinyobj::real_t nx =  attrib.vertices[3 * idx.normal_index + 0];
                tinyobj::real_t ny =  attrib.vertices[3 * idx.normal_index + 1];
                tinyobj::real_t nz =  attrib.vertices[3 * idx.normal_index + 2];

                tinyobj::real_t tx, ty;

                if (!attrib.texcoords.empty())
                {
                    tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                    ty = attrib.texcoords[2 * idx.texcoord_index + 1];
                }
                else
                {
                    if (v == 0)
                    {
                        tx = ty = 0;
                    }
                    else if (v == 1)
                    {
                        tx = 0, ty = 1;
                    }
                    else
                    {
                        tx = ty = 1;
                    }
                }

                mVertices.push_back(Vec4f(vx, vy, vz, tx));
                mNormals.push_back(Vec4f(nx, ny, nz, ty));
            }

            indexOffset += 3;
        }
    }

    return (true);
}

///////////////////////////////////////////////////////////////////////////////
const SharedPtr<Bvh>& Mesh::GetBVH(void) const
{
    return (mBvh);
}

///////////////////////////////////////////////////////////////////////////////
void Mesh::Build(void)
{
    const int numTris = mVertices.size() / 3;
    Vector<BBox> bounds(numTris);

    for (int i = 0; i < numTris; i++)
    {
        const Vec3f v1 = Vec3f(mVertices[i * 3 + 0]);
        const Vec3f v2 = Vec3f(mVertices[i * 3 + 1]);
        const Vec3f v3 = Vec3f(mVertices[i * 3 + 2]);

        bounds[i].Grow(v1);
        bounds[i].Grow(v2);
        bounds[i].Grow(v3);
    }

    mBvh->Build(bounds);
}

///////////////////////////////////////////////////////////////////////////////
String Mesh::GetName(void) const
{
    return (mName);
}

///////////////////////////////////////////////////////////////////////////////
const Vector<Vec4f>& Mesh::GetVertices(void) const
{
    return (mVertices);
}

///////////////////////////////////////////////////////////////////////////////
const Vector<Vec4f>& Mesh::GetNormals(void) const
{
    return (mNormals);
}

} // namespace Ray

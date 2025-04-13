///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#define TINYOBJLOADER_IMPLEMENTATION
#include "Resources/Mesh.hpp"
#include "Utils/Exception.hpp"
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
Mesh::Mesh(void)
{

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

        for (const auto& face : shape.mesh.num_face_vertices)
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
void Mesh::Build(void)
{
    const int numTris = mVertices.size() / 3;
    Vector<BBox> bounds(numTris);

#pragma omp parallel for
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

} // namespace Ray


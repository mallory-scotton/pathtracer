///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#define TINYOBJLOADER_IMPLEMENTATION

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "ImGui/tiny_obj_loader.h"
#include "Components/Mesh.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
Mesh::Mesh(void)
    : bvh(std::make_unique<Ray::SplitBvh>(2.0f, 64, 0, 0.001f, 0))
{}

///////////////////////////////////////////////////////////////////////////////
Mesh::~Mesh()
{
    bvh.reset();
}

///////////////////////////////////////////////////////////////////////////////
bool Mesh::LoadFromFile(const String& filename)
{
    name = filename;
    tinyobj::attrib_t attrib;
    Vector<tinyobj::shape_t> shapes;
    Vector<tinyobj::material_t> materials;
    String err;

    bool ret = tinyobj::LoadObj(
        &attrib,
        &shapes,
        &materials,
        &err,
        &err,
        filename.c_str()
    );

    if (!ret)
    {
        RAY_ERROR("Unable to load model: \"" << filename << "\"");
        return (false);
    }

    for (size_t s = 0; s < shapes.size(); s++)
    {
        size_t index_offset = 0;

        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            if (shapes[s].mesh.num_face_vertices[f] != 3) {
                RAY_WARN(
                    "Warning: Non-triangular face detected. Skipping face."
                );
                index_offset += shapes[s].mesh.num_face_vertices[f];
                continue;
            }

            for (size_t v = 0; v < 3; v++)
            {
                if (index_offset + v >= shapes[s].mesh.indices.size())
                {
                    RAY_ERROR(
                        "Error: Index out of bounds accessing shape indices."
                    );
                    return (false);
                }

                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                if (
                    idx.vertex_index < 0 ||
                    3 * idx.vertex_index + 2 >= (int)attrib.vertices.size()
                )
                {
                    RAY_ERROR("Error: Invalid vertex index.");
                    return (false);
                }

                tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];

                tinyobj::real_t nx = 0.0f, ny = 0.0f, nz = 1.0f;

                if (
                    idx.normal_index >= 0 &&
                    3 * idx.normal_index + 2 < (int)attrib.normals.size()
                )
                {
                    nx = attrib.normals[3 * idx.normal_index + 0];
                    ny = attrib.normals[3 * idx.normal_index + 1];
                    nz = attrib.normals[3 * idx.normal_index + 2];
                }
                else if (!attrib.normals.empty())
                {
                    RAY_WARN(
                        "Warning: Invalid normal index, using default normal."
                    );
                }

                tinyobj::real_t tx = 0.0f, ty = 0.0f;

                if (
                    !attrib.texcoords.empty() && idx.texcoord_index >= 0 &&
                    2 * idx.texcoord_index + 1 < (int)attrib.texcoords.size()
                )
                {
                    tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                    ty = 1.0f - attrib.texcoords[2 * idx.texcoord_index + 1];
                }
                else
                {
                    if (v == 0)
                    {
                        tx = ty = 0.0f;
                    } else if (v == 1)
                    {
                        tx = 0.0f;
                        ty = 1.0f;
                    } else
                    {
                        tx = ty = 1.0f;
                    }
                }

                verticesUVX.push_back(Vec4f(vx, vy, vz, tx));
                normalsUVY.push_back(Vec4f(nx, ny, nz, ty));
            }

            index_offset += 3;
        }
    }

    return (true);
}

///////////////////////////////////////////////////////////////////////////////
void Mesh::BuildBVH(void)
{
    const int numTris = verticesUVX.size() / 3;
    Vector<Ray::BoundingBox> bounds(numTris);

    for (int i = 0; i < numTris; ++i)
    {
        const Vec3f v1 = Vec3f(verticesUVX[i * 3 + 0]);
        const Vec3f v2 = Vec3f(verticesUVX[i * 3 + 1]);
        const Vec3f v3 = Vec3f(verticesUVX[i * 3 + 2]);

        bounds[i].Grow(v1);
        bounds[i].Grow(v2);
        bounds[i].Grow(v3);
    }

    bvh->Build(&bounds[0], numTris);
}

///////////////////////////////////////////////////////////////////////////////
MeshInstance::MeshInstance(
    const String& name,
    int meshID,
    Mat4x4f transform,
    int materialID
)
    : transform(transform)
    , name(name)
    , materialID(materialID)
    , meshID(meshID)
{}

}

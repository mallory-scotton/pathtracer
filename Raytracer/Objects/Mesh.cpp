///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#define TINYOBJLOADER_IMPLEMENTATION

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "ImGui/tiny_obj_loader.h"
#include "Objects/Mesh.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespacey Ray::Objects
///////////////////////////////////////////////////////////////////////////////
namespace Ray::Objects
{

///////////////////////////////////////////////////////////////////////////////
Mesh::Mesh(void)
    : AObject()
{}

///////////////////////////////////////////////////////////////////////////////
Mesh::Mesh(const Path& filepath)
    : AObject()
{
    LoadFromFile(filepath);
}

///////////////////////////////////////////////////////////////////////////////
bool Mesh::LoadFromFile(const Path& filepath)
{
    m_name = filepath.filename();
    m_hash = filepath;
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
        filepath.c_str()
    );

    if (!ret)
    {
        RAY_ERROR("Unable to load model: " << filepath);
        return (false);
    }

    for (Uint64 s = 0; s < shapes.size(); s++)
    {
        Uint64 index_offset = 0;

        for (Uint64 f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            if (shapes[s].mesh.num_face_vertices[f] != 3) {
                RAY_WARN(
                    "Warning: Non-triangular face detected. Skipping face."
                );
                index_offset += shapes[s].mesh.num_face_vertices[f];
                continue;
            }

            for (Uint64 v = 0; v < 3; v++)
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

                m_vertices.push_back(Vec4f(vx, vy, vz, tx));
                m_normals.push_back(Vec4f(nx, ny, nz, ty));
            }

            index_offset += 3;
        }
    }

    return (true);
}

} // namespace Ray::Objects

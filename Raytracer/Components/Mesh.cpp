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

// FIXME: C FUNCTION
///////////////////////////////////////////////////////////////////////////////
float sphericalTheta(const Vec3f& v)
{
    return (acosf(Math::Clamp(v.y, -1.f, 1.f)));
}

// FIXME: C FUNCTION
///////////////////////////////////////////////////////////////////////////////
float sphericalPhi(const Vec3f& v)
{
    float p = atan2f(v.z, v.x);
    return ((p < 0.f) ? p + 2.f * PI : p);
}

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
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err,
        filename.c_str(), 0, true);

    if (!ret)
    {
        printf("Unable to load model\n");
        return (false);
    }

    for (size_t s = 0; s < shapes.size(); s++)
    {
        size_t index_offset = 0;

        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            for (size_t v = 0; v < 3; v++)
            {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
                tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
                tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
                tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];

                tinyobj::real_t tx, ty;

                if (!attrib.texcoords.empty())
                {
                    tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                    ty = 1.0 - attrib.texcoords[2 * idx.texcoord_index + 1];
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

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#define STB_IMAGE_RESIZE_IMPLEMENTATION

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "ImGui/stb_image_resize.h"
#include "ImGui/stb_image.h"
#include "Components/Scene.hpp"
#include "Components/Camera.hpp"
#include "Objects/Mesh.hpp"
#include "Objects.hpp"
#include "Factories/PrimitiveFactory.hpp"
#include "Utils/Utils.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
Scene::Scene(void)
    : envMap(nullptr)
    , camera(nullptr)
    , initialized(false)
    , dirty(true)
    , instancesModified(false)
    , envMapModified(false)
{
    sceneBvh = std::make_unique<Ray::Bvh>(10.0f, 64, false);
}

///////////////////////////////////////////////////////////////////////////////
Scene::~Scene()
{
    for (int i = 0; i < static_cast<int>(textures.size()); i++)
    {
        delete textures[i];
    }

    textures.clear();

    if (envMap)
    {
        delete envMap;
    }
};

///////////////////////////////////////////////////////////////////////////////
int Scene::AddMesh(const Path& filename)
{
    int id = -1;

    for (int i = 0; i < static_cast<int>(objects.size()); i++)
    {
        if (objects[i]->GetName() == filename)
        {
            return (i);
        }
    }

    id = static_cast<int>(objects.size());

    RAY_TRACE("Loading Model: " << filename);
    objects.push_back(std::make_unique<Objects::Mesh>(filename));

    return (id);
}

///////////////////////////////////////////////////////////////////////////////
int Scene::AddTexture(const Path& filename)
{
    int id = -1;

    for (int i = 0; i < static_cast<int>(textures.size()); i++)
    {
        if (textures[i]->name == filename)
        {
            return (i);
        }
    }

    id = static_cast<int>(textures.size());
    Texture* texture = new Texture;

    RAY_TRACE("Loading texture " << filename);
    if (texture->LoadTexture(filename))
    {
        textures.push_back(texture);
    }
    else
    {
        RAY_WARN("Unable to load texture " << filename);
        delete texture;
        id = -1;
    }

    return (id);
}

///////////////////////////////////////////////////////////////////////////////
int Scene::AddMaterial(const Material& material, String name)
{
    int id = static_cast<int>(materials.size());
    materials.push_back(material);
    materialsName.push_back(name);
    return (id);
}

///////////////////////////////////////////////////////////////////////////////
int Scene::getMaterialID(const String& name)
{
    auto it = std::find(materialsName.begin(), materialsName.end(), name);

    if (it != materialsName.end())
    {
        return (static_cast<int>(std::distance(materialsName.begin(), it)));
    }
    else
    {
        RAY_WARN("Could not find material " << name);
        return (-1);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Scene::AddEnvMap(const Path& filename)
{
    if (envMap)
    {
        delete envMap;
    }

    envMap = new EnvironmentMap;
    if (envMap->LoadMap(filename))
    {
        RAY_TRACE("HDR " << filename << " loaded");
    }
    else
    {
        RAY_WARN("Unable to load HDR " << filename);
        delete envMap;
        envMap = nullptr;
    }
    envMapModified = true;
    dirty = true;
}

///////////////////////////////////////////////////////////////////////////////
int Scene::AddMeshInstance(const Instance& instance)
{
    int id = static_cast<int>(instances.size());
    instances.push_back(instance);
    return (id);
}

///////////////////////////////////////////////////////////////////////////////
int Scene::AddLight(const Light& light)
{
    int id = static_cast<int>(lights.size());
    lights.push_back(light);
    return (id);
}

///////////////////////////////////////////////////////////////////////////////
bool Scene::CanUpdate(int sampleCounter) const
{
    return (!(
        !dirty &&
        renderOptions.maxSpp != -1 &&
        sampleCounter >= renderOptions.maxSpp
    ));
}

///////////////////////////////////////////////////////////////////////////////
void Scene::createTLAS(void)
{
    std::vector<Ray::BoundingBox> bounds;
    bounds.resize(instances.size());

    for (Uint64 i = 0; i < instances.size(); i++)
    {
        Ray::BoundingBox BoundingBox = objects[instances[i].objectID]->GetBVH()->Bounds();
        Mat4x4f matrix = instances[i].transform;

        Vec3f minBound = BoundingBox.min;
        Vec3f maxBound = BoundingBox.max;

        Vec3f right       = Vec3f(matrix[0][0], matrix[0][1], matrix[0][2]);
        Vec3f up          = Vec3f(matrix[1][0], matrix[1][1], matrix[1][2]);
        Vec3f forward     = Vec3f(matrix[2][0], matrix[2][1], matrix[2][2]);
        Vec3f translation = Vec3f(matrix[3][0], matrix[3][1], matrix[3][2]);

        Vec3f xa = right * minBound.x;
        Vec3f xb = right * maxBound.x;

        Vec3f ya = up * minBound.y;
        Vec3f yb = up * maxBound.y;

        Vec3f za = forward * minBound.z;
        Vec3f zb = forward * maxBound.z;

        minBound = Vec3f::Min(xa, xb) + Vec3f::Min(ya, yb) + Vec3f::Min(za, zb) + translation;
        maxBound = Vec3f::Max(xa, xb) + Vec3f::Max(ya, yb) + Vec3f::Max(za, zb) + translation;

        Ray::BoundingBox bound;
        bound.min = minBound;
        bound.max = maxBound;

        bounds[i] = bound;
    }
    sceneBvh->Build(&bounds[0], bounds.size());
    sceneBounds = sceneBvh->Bounds();
}

///////////////////////////////////////////////////////////////////////////////
void Scene::createBLAS()
{
    for (Uint64 i = 0; i < objects.size(); i++)
    {
        RAY_TRACE("Building BVH for " << objects[i]->GetName());
        objects[i]->BuildBVH();
    }
}

///////////////////////////////////////////////////////////////////////////////
void Scene::RebuildInstances(void)
{
    sceneBvh = std::make_unique<Ray::Bvh>(10.0f, 64, false);

    createTLAS();
    bvhTranslator.UpdateTLAS(sceneBvh.get(), instances);

    for (Uint64 i = 0; i < instances.size(); i++)
    {
        transforms[i] = instances[i].transform;
    }

    instancesModified = true;
    dirty = true;
}

///////////////////////////////////////////////////////////////////////////////
void Scene::ProcessScene()
{
    RAY_TRACE("Processing scene data");
    createBLAS();

    RAY_TRACE("Building scene BVH");
    createTLAS();

    // Flatten BVH
    RAY_TRACE("Flattening BVH");
    bvhTranslator.Process(sceneBvh.get(), objects, instances);

    // Copy mesh data
    int verticesCnt = 0;
    RAY_TRACE("Copying Mesh Data");
    for (Uint64 i = 0; i < objects.size(); i++)
    {
        Uint64 numIndices = objects[i]->GetBVH()->GetNumIndices();
        const int* triIndices = objects[i]->GetBVH()->GetIndices();

        for (Uint64 j = 0; j < numIndices; j++)
        {
            int index = triIndices[j];
            int v1 = (index * 3 + 0) + verticesCnt;
            int v2 = (index * 3 + 1) + verticesCnt;
            int v3 = (index * 3 + 2) + verticesCnt;

            vertIndices.push_back(Index{ v1, v2, v3 });
        }

        verticesUVX.insert(verticesUVX.end(), objects[i]->GetVertices().begin(), objects[i]->GetVertices().end());
        normalsUVY.insert(normalsUVY.end(), objects[i]->GetNormals().begin(), objects[i]->GetNormals().end());

        verticesCnt += objects[i]->GetVertices().size();
    }

    // Copy transforms
    RAY_TRACE("Copying transforms");
    transforms.resize(instances.size());
    for (Uint64 i = 0; i < instances.size(); i++)
    {
        transforms[i] = instances[i].transform;
    }

    if (!textures.empty())
    {
        RAY_TRACE("Copying and resizing textures");
    }

    int reqWidth = renderOptions.texArrayWidth;
    int reqHeight = renderOptions.texArrayHeight;
    int texBytes = reqWidth * reqHeight * 4;
    textureMapsArray.resize(texBytes * textures.size());

    for (Uint64 i = 0; i < textures.size(); i++)
    {
        int texWidth = textures[i]->width;
        int texHeight = textures[i]->height;

        if (texWidth != reqWidth || texHeight != reqHeight)
        {
            unsigned char* resizedTex = new unsigned char[texBytes];
            stbir_resize_uint8(&textures[i]->texData[0], texWidth, texHeight, 0, resizedTex, reqWidth, reqHeight, 0, 4);
            std::copy(resizedTex, resizedTex + texBytes, &textureMapsArray[i * texBytes]);
            delete[] resizedTex;
        }
        else
        {
            std::copy(textures[i]->texData.begin(), textures[i]->texData.end(), &textureMapsArray[i * texBytes]);
        }
    }

    if (!camera)
    {
        Ray::BoundingBox bounds = sceneBvh->Bounds();
        Vec3f extents = bounds.Extents();
        Vec3f center = bounds.Center();
        camera = std::make_unique<Camera>(Vec3f(center.x, center.y, center.z + Vec3f::Length(extents) * 2.0f), center, 45.0f);
    }

    initialized = true;
}

} // namespace Ray

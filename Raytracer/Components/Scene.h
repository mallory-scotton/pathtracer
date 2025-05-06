// FIXME: THE WHOLE FILE

#pragma once

#include <string>
#include <vector>
#include <map>
#include "Graphics/EnvironmentMap.hpp"
#include "Acceleration/bvh.h"
#include "Core/Renderer.hpp"
#include "Components/Mesh.hpp"
#include "Components/Camera.hpp"
#include "Acceleration/bvh_translator.h"
#include "Graphics/Texture.hpp"
#include "Components/Material.hpp"
#include "Components/Light.hpp"

namespace Ray
{
    class Camera;

    struct Indices
    {
        int x, y, z;
    };

    class Scene
    {
    public:
        Scene()
            : envMap(nullptr)
            , camera(nullptr)
            , initialized(false)
            , dirty(true)
        {
            sceneBvh = new Ray::Bvh(10.0f, 64, false);
        }
        ~Scene();

        int AddMesh(const std::string& filename);
        int AddTexture(const std::string& filename);
        int AddMaterial(const Material& material);
        int AddMeshInstance(const MeshInstance& meshInstance);
        int AddLight(const Light& light);

        void AddEnvMap(const std::string& filename);

        void ProcessScene();
        void RebuildInstances();

        // Options
        RenderOptions renderOptions;

        // Meshes
        std::vector<Mesh*> meshes;

        // Scene Mesh Data 
        std::vector<Indices> vertIndices;
        std::vector<Vec4f> verticesUVX; // Vertex + texture Coord (u/s)
        std::vector<Vec4f> normalsUVY; // Normal + texture Coord (v/t)
        std::vector<Mat4x4f> transforms;

        // Materials
        std::vector<Material> materials;

        // Instances
        std::vector<MeshInstance> meshInstances;

        // Lights
        std::vector<Light> lights;

        // Environment Map
        EnvironmentMap* envMap;

        // Camera
        UniquePtr<Camera> camera;

        // Bvh
        Ray::BvhTranslator bvhTranslator; // Produces a flat bvh array for GPU consumption
        Ray::BoundingBox sceneBounds;

        // Texture Data
        std::vector<Texture*> textures;
        std::vector<unsigned char> textureMapsArray;

        bool initialized;
        bool dirty;
        // To check if scene elements need to be resent to GPU
        bool instancesModified = false;
        bool envMapModified = false;

    private:
        Ray::Bvh* sceneBvh;
        void createBLAS();
        void createTLAS();
    };
}

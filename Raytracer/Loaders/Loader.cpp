// FIXME: THE WHOLE FILE
///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Loaders/Loader.hpp"
#include "Loaders/GLTFLoader.h"
#include "Utils/LibConfig.hpp"
#include "Utils/FileSystem.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
void Loader::ParseSceneMaterial(
    const LibConfig::Setting& cfg,
    Map<String, MaterialData>& materials,
    Scene* scene
)
{
    String albedo, metallicRoughness, normal, emission, alpha, medium, name;
    Material material;

    cfg.Value("name", name);
    cfg.Value("color", material.baseColor);
    cfg.Value("opacity", material.opacity);
    cfg.Value("alphamode", alpha);
    cfg.Value("alphacutoff", material.alphaCutoff);
    cfg.Value("emission", material.emission);
    cfg.Value("metallic", material.metallic);
    cfg.Value("roughness", material.roughness);
    cfg.Value("subsurface", material.subsurface);
    cfg.Value("speculartint", material.specularTint);
    cfg.Value("anisotropic", material.anisotropic);
    cfg.Value("sheen", material.sheen);
    cfg.Value("sheentint", material.sheenTint);
    cfg.Value("clearcoat", material.clearcoat);
    cfg.Value("clearcoatgloss", material.clearcoatGloss);
    cfg.Value("spectrans", material.specTrans);
    cfg.Value("ior", material.ior);
    cfg.Value("albedotexture", albedo);
    cfg.Value("metallicroughnesstexture", metallicRoughness);
    cfg.Value("normaltexture", normal);
    cfg.Value("emissiontexture", emission);
    cfg.Value("mediumtype", medium);
    cfg.Value("mediumdensity", material.mediumDensity);
    cfg.Value("mediumcolor", material.mediumColor);
    cfg.Value("mediumanisotropy", material.mediumAnisotropy);

    if (!albedo.empty() && albedo != "none")
    {
        material.baseColorTexId = scene->AddTexture(albedo);
    }
    if (!metallicRoughness.empty() && metallicRoughness != "none")
    {
        material.metallicRoughnessTexID = scene->AddTexture(metallicRoughness);
    }
    if (!normal.empty() && normal != "none")
    {
        material.normalmapTexID = scene->AddTexture(normal);
    }
    if (!emission.empty() && emission != "none")
    {
        material.emissionmapTexID = scene->AddTexture(emission);
    }

    if (alpha == "opaque")
    {
        material.alphaMode = Material::OPAQUE;
    }
    else if (alpha == "blend")
    {
        material.alphaMode = Material::BLEND;
    }
    else if (alpha == "mask")
    {
        material.alphaMode = Material::MASK;
    }

    if (medium == "absorb")
    {
        material.mediumType = Material::ABSORB;
    }
    else if (medium == "scatter")
    {
        material.mediumType = Material::SCATTER;
    }
    else if (medium == "emissive")
    {
        material.mediumType = Material::EMISSIVE;
    }

    if (materials.find(name) == materials.end())
    {
        int id = scene->AddMaterial(material);
        materials[name] = MaterialData{material, id};
    }
}

///////////////////////////////////////////////////////////////////////////////
void Loader::ParseSceneRendererOptions(
    const LibConfig::Setting& cfg,
    RenderOptions& options,
    Scene* scene
)
{
    String envMap;

    cfg.Value("envmapfile", envMap);
    cfg.Value("resolution", options.renderResolution);
    cfg.Value("windowresolution", options.windowResolution);
    cfg.Value("envmapintensity", options.envMapIntensity);
    cfg.Value("maxdepth", options.maxDepth);
    cfg.Value("maxssp", options.maxSpp);
    cfg.Value("tilewidth", options.tileWidth);
    cfg.Value("tileheight", options.tileHeight);
    cfg.Value("enablerr", options.enableRR);
    cfg.Value("rrdepth", options.RRDepth);
    cfg.Value("enabletonemap", options.enableTonemap);
    cfg.Value("enableaces", options.enableAces);
    cfg.Value("texarraywidth", options.texArrayWidth);
    cfg.Value("texarrayheight", options.texArrayHeight);
    cfg.Value("openglnormalmap", options.openglNormalMap);
    cfg.Value("hideemitters", options.hideEmitters);
    cfg.Value("enablebackground", options.enableBackground);
    cfg.Value("transparentbackground", options.transparentBackground);
    cfg.Value("backgroundcolor", options.backgroundCol);
    cfg.Value("independentrendersize", options.independentRenderSize);
    cfg.Value("envmaprotation", options.envMapRot);
    cfg.Value("enableroughnessmollification",
        options.enableRoughnessMollification);
    cfg.Value("roughnessmollificationamt", options.roughnessMollificationAmt);
    cfg.Value("enablevolumemis", options.enableVolumeMIS);
    cfg.Value("enableuniformlight", options.enableUniformLight);
    cfg.Value("uniformlightcolor", options.uniformLightCol);

    if (!envMap.empty() && envMap != "none")
    {
        scene->AddEnvMap(envMap);
        options.enableEnvMap = true;
    }
    else
    {
        options.enableEnvMap = false;
    }

    if (!options.independentRenderSize)
    {
        options.windowResolution = options.renderResolution;
    }
}

// FIXME: C POINTERS
///////////////////////////////////////////////////////////////////////////////
void Loader::ParseSceneCamera(const LibConfig::Setting& cfg, Scene* scene)
{
    Vec3f position, lookAt;
    float fov, aperture = 0.f, focalDist = 1.f;

    cfg.Value("position", position);
    cfg.Value("lookat", lookAt);
    cfg.Value("fov", fov);
    cfg.Value("aperture", aperture);
    cfg.Value("focalDist", focalDist);

    delete scene->camera;

    scene->AddCamera(position, lookAt, fov);
    scene->camera->aperture = aperture;
    scene->camera->focalDist = focalDist;
}

///////////////////////////////////////////////////////////////////////////////
void Loader::ParseSceneLight(const LibConfig::Setting& cfg, Scene* scene)
{
    Light light;
    Vec3f v1, v2;
    String lightType;

    cfg.Value("position", light.position);
    cfg.Value("emission", light.emission);
    cfg.Value("radius", light.radius);
    cfg.Value("v1", v1);
    cfg.Value("v2", v2);
    cfg.Value("type", lightType);

    if (lightType == "quad")
    {
        light.type = Light::RECTANGLE;
        light.u = v1 - light.position;
        light.v = v2 - light.position;
        light.area = Vec3f::Length(Vec3f::Cross(light.u, light.v));
    }
    else if (lightType == "sphere")
    {
        light.type = Light::SPHERE;
        light.area = 4.f * PI * light.radius * light.radius;
    }
    else if (lightType == "distant")
    {
        light.type = Light::DISTANT;
        light.area = 0.f;
    }

    scene->AddLight(light);
}

///////////////////////////////////////////////////////////////////////////////
void Loader::ParseSceneMesh(
    const LibConfig::Setting& cfg,
    Map<String, MaterialData>& materials,
    Scene* scene
)
{
    String file, name, material;
    Vec3f position, scale;
    Quaternionf rotation;
    Mat4x4f matRot, matTrans, matScale;
    int materialID = 0;

    cfg.Value("name", name);
    cfg.Value("file", file);

    if (cfg.Value("material", material))
    {
        if (materials.find(material) != materials.end())
        {
            materialID = materials[material].id;
        }
        else
        {
            RAY_WARN("Could not find material " << material);
        }
    }

    if (cfg.Value("position", position))
    {
        matTrans[3][0] = position.x;
        matTrans[3][1] = position.y;
        matTrans[3][2] = position.z;
    }

    if (cfg.Value("scale", scale))
    {
        matScale[0][0] = scale.x;
        matScale[1][1] = scale.y;
        matScale[2][2] = scale.z;
    }

    if (cfg.Value("rotation", rotation))
    {
        matRot = Mat4x4f::QuaternionToMatrix(rotation);
    }

    if (!file.empty())
    {
        int id = scene->AddMesh(file);

        if (id != -1)
        {
            String instanceName;

            if (!name.empty() && name != "none")
            {
                instanceName = name;
            }
            else
            {
                size_t pos = file.find_last_of("/\\");
                instanceName = file.substr(pos + 1);
            }

            Mat4x4f transform;

            transform = matRot * matScale * matTrans;

            MeshInstance instance(instanceName, id, transform, materialID);
            scene->AddMeshInstance(instance);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Loader::ParseSceneGLTF(
    const LibConfig::Setting& cfg,
    Scene* scene,
    RenderOptions& options
)
{
    String file;
    Vec3f position, scale;
    Quaternionf rotation;
    Mat4x4f matRot, matTrans, matScale;

    cfg.Value("file", file);

    if (cfg.Value("position", position))
    {
        matTrans[3][0] = position.x;
        matTrans[3][1] = position.y;
        matTrans[3][2] = position.z;
    }

    if (cfg.Value("scale", scale))
    {
        matScale[0][0] = scale.x;
        matScale[1][1] = scale.y;
        matScale[2][2] = scale.z;
    }

    if (cfg.Value("rotation", rotation))
    {
        matRot = Mat4x4f::QuaternionToMatrix(rotation);
    }

    if (!file.empty())
    {
        Mat4x4f transform;

        transform = matRot * matScale * matTrans;

        String extension = Fs::GetExtension(file);

        if (extension == ".gltf")
        {
            LoadGLTF(file, scene, options, transform, false);
        }
        else if (extension == ".glb")
        {
            LoadGLTF(file, scene, options, transform, true);
        }
        else
        {
            RAY_WARN("Unsupported file format: " << extension);
        }
    }
}

// FIXME: C POINTERS
///////////////////////////////////////////////////////////////////////////////
bool Loader::LoadScene(
    const Path& filename,
    Scene* scene,
    RenderOptions& options
)
{
    RAY_TRACE("Loading Scene: " << filename);

    LibConfig config(filename);

    Map<String, MaterialData> materialMap;

    scene->AddMaterial(Material());

    RAY_INFO("RENDERER");

    if (const auto& renderer = config.Lookup("renderer"))
    {
        ParseSceneRendererOptions(*renderer, options, scene);
    }

    RAY_INFO("CAMERA");
    if (const auto& camera = config.Lookup("camera"))
    {
        ParseSceneCamera(*camera, scene);
    }

    if (const auto& materials = config.Lookup("materials"))
    {
        for (int i = 0; i < materials->Length(); i++)
        {
            ParseSceneMaterial(materials->At(i), materialMap, scene);
        }
    }

    RAY_INFO("MESHES");
    if (const auto& meshes = config.Lookup("meshes"))
    {
        for (int i = 0; i < meshes->Length(); i++)
        {
            ParseSceneMesh(meshes->At(i), materialMap, scene);
        }
    }

    RAY_INFO("LIGHTS");
    if (const auto& lights = config.Lookup("lights"))
    {
        for (int i = 0; i < lights->Length(); i++)
        {
            ParseSceneLight(lights->At(i), scene);
        }
    }

    RAY_INFO("GLTFS");
    if (const auto& gltfs = config.Lookup("gltfs"))
    {
        for (int i = 0; i < gltfs->Length(); i++)
        {
            ParseSceneGLTF(gltfs->At(i), scene, options);
        }
    }

    return (true);
}

}  // namespace Ray

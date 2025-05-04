// FIXME: THE WHOLE FILE
///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Loaders/Loader.hpp"
#include "Loaders/GLTFLoader.h"
#include "Utils/LibConfig.hpp"
#include "Utils/FileSystem.hpp"
#include "Builders/CameraBuilder.hpp"
#include "Core/Context.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
void Loader::ParseSceneMaterial(
    const LibConfig::Setting& cfg,
    Map<String, MaterialData>& materials
)
{
    Context& ctx = Context::GetInstance();
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
        material.baseColorTexId = ctx.scene->AddTexture(albedo);
    }
    if (!metallicRoughness.empty() && metallicRoughness != "none")
    {
        material.metallicRoughnessTexID = ctx.scene->AddTexture(metallicRoughness);
    }
    if (!normal.empty() && normal != "none")
    {
        material.normalmapTexID = ctx.scene->AddTexture(normal);
    }
    if (!emission.empty() && emission != "none")
    {
        material.emissionmapTexID = ctx.scene->AddTexture(emission);
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
        int id = ctx.scene->AddMaterial(material);
        materials[name] = MaterialData{material, id};
    }
}

///////////////////////////////////////////////////////////////////////////////
void Loader::ParseSceneRendererOptions(const LibConfig::Setting& cfg)
{
    Context& ctx = Context::GetInstance();
    RenderOptions& options = ctx.scene->renderOptions;
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
        ctx.scene->AddEnvMap(envMap);
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
void Loader::ParseSceneCamera(const LibConfig::Setting& cfg)
{
    CameraBuilder builder;
    Context& ctx = Context::GetInstance();

    ctx.scene->camera = std::make_unique<Camera>(
        builder.FromConfiguration(cfg).Build()
    );
}

///////////////////////////////////////////////////////////////////////////////
void Loader::ParseSceneLight(const LibConfig::Setting& cfg)
{
    Context& ctx = Context::GetInstance();
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

    ctx.scene->AddLight(light);
}

///////////////////////////////////////////////////////////////////////////////
void Loader::ParseSceneMesh(
    const LibConfig::Setting& cfg,
    Map<String, MaterialData>& materials
)
{
    Context& ctx = Context::GetInstance();
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
        int id = ctx.scene->AddMesh(file);

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
            ctx.scene->AddMeshInstance(instance);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Loader::ParseSceneGLTF(const LibConfig::Setting& cfg)
{
    Context& ctx = Context::GetInstance();
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
            LoadGLTF(
                file,
                ctx.scene.get(),
                ctx.scene->renderOptions,
                transform,
                false
            );
        }
        else if (extension == ".glb")
        {
            LoadGLTF(
                file,
                ctx.scene.get(),
                ctx.scene->renderOptions,
                transform,
                true
            );
        }
        else
        {
            RAY_WARN("Unsupported file format: " << extension);
        }
    }
}

// FIXME: C POINTERS
///////////////////////////////////////////////////////////////////////////////
bool Loader::LoadScene(const Path& filename)
{
    Context& ctx = Context::GetInstance();
    RAY_TRACE("Loading Scene: " << filename);

    LibConfig config(filename);

    Map<String, MaterialData> materialMap;

    ctx.scene->AddMaterial(Material());

    if (const auto& renderer = config.Lookup("renderer"))
    {
        ParseSceneRendererOptions(*renderer);
    }

    if (const auto& camera = config.Lookup("camera"))
    {
        ParseSceneCamera(*camera);
    }

    if (const auto& materials = config.Lookup("materials"))
    {
        for (int i = 0; i < materials->Length(); i++)
        {
            ParseSceneMaterial(materials->At(i), materialMap);
        }
    }

    if (const auto& meshes = config.Lookup("meshes"))
    {
        for (int i = 0; i < meshes->Length(); i++)
        {
            ParseSceneMesh(meshes->At(i), materialMap);
        }
    }

    if (const auto& lights = config.Lookup("lights"))
    {
        for (int i = 0; i < lights->Length(); i++)
        {
            ParseSceneLight(lights->At(i));
        }
    }

    if (const auto& gltfs = config.Lookup("gltfs"))
    {
        for (int i = 0; i < gltfs->Length(); i++)
        {
            ParseSceneGLTF(gltfs->At(i));
        }
    }

    return (true);
}

}  // namespace Ray

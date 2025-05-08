// FIXME: THE WHOLE FILE
///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Loaders/Loader.hpp"
#include "Loaders/GLTFLoader.h"
#include "Utils/LibConfig.hpp"
#include "Utils/FileSystem.hpp"
#include "Builders/CameraBuilder.hpp"
#include "Builders/LightBuilder.hpp"
#include "Builders/MaterialBuilder.hpp"
#include "Builders/MeshInstanceBuilder.hpp"
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
    MaterialBuilder builder;
    builder.FromConfiguration(cfg);

    if (materials.find(builder.GetName()) == materials.end())
    {
        int id = ctx.scene->AddMaterial(builder.Build(), builder.GetName());
        materials[builder.GetName()] = MaterialData{builder.Build(), id};
    }
}

///////////////////////////////////////////////////////////////////////////////
void Loader::ParseSceneRendererOptions(const LibConfig::Setting& cfg)
{
    Context& ctx = Context::GetInstance();
    Renderer::Options& options = ctx.scene->renderOptions;
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
    LightBuilder builder;

    ctx.scene->AddLight(builder.FromConfiguration(cfg).Build());
}

/////////////////////////////////////////////////////////////////////////////
void Loader::ParseSceneMesh(
    const LibConfig::Setting& cfg,
    Map<String, MaterialData>& materials
)
{
    Context& ctx = Context::GetInstance();
    MeshInstanceBuilder builder;

    builder.FromConfiguration(cfg);

    if (builder.IDCheck())
    {
    ctx.scene->AddMeshInstance(builder.Build());
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

    ctx.scene->AddMaterial(Material(), "default");

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
    std::cout << "meshesInstance: " << ctx.scene->meshInstances.size() << std::endl;
    std::cout << "meshes: " << ctx.scene->meshes.size() << std::endl;
    return (true);
}

}  // namespace Ray

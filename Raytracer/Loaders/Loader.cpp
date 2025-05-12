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
#include "Builders/PrimitiveBuilder.hpp"
#include "Builders/RendererOptionsBuilder.hpp"
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
    RendererOptionsBuilder builder;

    ctx.scene->renderOptions = builder.FromConfiguration(cfg).Build();
}


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
void Loader::ParseSceneMesh(const LibConfig::Setting& cfg)
{
    Context& ctx = Context::GetInstance();
    MeshInstanceBuilder builder;

    builder.FromConfiguration(cfg);

    if (builder.IDCheck())
    {
        ctx.scene->AddMeshInstance(builder.Build());
    }
}

/////////////////////////////////////////////////////////////////////////////
void Loader::ParseScenePrimitive(const LibConfig::Setting& cfg)
{
    Context& ctx = Context::GetInstance();
    PrimitiveBuilder builder;

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
            ParseSceneMesh(meshes->At(i));
        }
    }

    if (const auto& primitives = config.Lookup("primitives"))
    {
        for (int i = 0; i < primitives->Length(); i++)
        {
            ParseScenePrimitive(primitives->At(i));
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

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Context.hpp"
#include "Utils/FileSystem.hpp"
#include "Errors/Exception.hpp"
#include "Loaders/Loader.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
const Path Context::DEFAULT_SHADER_PATH = "./Shaders/";

///////////////////////////////////////////////////////////////////////////////
const Path Context::DEFAULT_SCENES_PATH = "./Scenes/";

///////////////////////////////////////////////////////////////////////////////
const Path Context::DEFAULT_ASSETS_PATH = "./Assets/";

///////////////////////////////////////////////////////////////////////////////
Context::Context(void)
    : m_shaderPath(DEFAULT_SHADER_PATH)
    , m_assetsPath(DEFAULT_ASSETS_PATH)
    , m_scenesPath(DEFAULT_SCENES_PATH)
    , scene(std::make_unique<Scene>())
    , shutdown(false)
    , hasGui(false)
    , renderTextureID(0)
{
    if (!Fs::Exists(m_shaderPath))
    {
        RAY_WARN(m_shaderPath << " doesn't exists !");
    }

    if (!Fs::Exists(m_assetsPath))
    {
        RAY_WARN(m_assetsPath << " doesn't exists !");
    }

    if (!Fs::Exists(m_scenesPath))
    {
        RAY_WARN(m_scenesPath << " doesn't exists !");
    }
}

///////////////////////////////////////////////////////////////////////////////
void Context::Initialize(int argc, char* argv[])
{
    RAY_TRACE("Initializing Context...");

    Vector<Path> scenes = Fs::DiscoverFilesByExtensions(
        m_scenesPath, {".scene"}
    );
    String sceneToLoad = "";

    for (const auto& sceneFile : scenes)
    {
        availableScenes.push_back(sceneFile);
    }

    if (
        argc > 1 &&
        Fs::Exists(argv[1]) &&
        Fs::GetExtension(argv[1]) == ".scene"
    )
    {
        sceneToLoad = argv[1];
    }
    else if (availableScenes.size() > 0)
    {
        sceneToLoad = availableScenes[0];
    }
    else
    {
        throw Exception("No default scene to load.");
    }

    Loader::LoadScene(sceneToLoad);

    int initialization = gl3wInit();

    if (initialization != 0)
    {
        throw Exception("Failed to initialize OpenGL loader");
    }

    renderer = std::make_unique<Renderer>();
}

///////////////////////////////////////////////////////////////////////////////
void Context::Shutdown(void)
{
    if (shutdown == false)
    {
        RAY_TRACE("Shuting down...");
        shutdown = true;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Context::SetShaderPath(const Path& path)
{
    if (Fs::Exists(path) && Fs::IsDirectory(path))
    {
        m_shaderPath = path;
    }
    else
    {
        RAY_WARN(path << " doesn't exists or is not a directory.");
    }
}

///////////////////////////////////////////////////////////////////////////////
void Context::SetAssetsPath(const Path& path)
{
    if (Fs::Exists(path) && Fs::IsDirectory(path))
    {
        m_assetsPath = path;
    }
    else
    {
        RAY_WARN(path << " doesn't exists or is not a directory.");
    }
}

///////////////////////////////////////////////////////////////////////////////
void Context::SetScenesPath(const Path& path)
{
    if (Fs::Exists(path) && Fs::IsDirectory(path))
    {
        m_scenesPath = path;
    }
    else
    {
        RAY_WARN(path << " doesn't exists or is not a directory.");
    }
}

///////////////////////////////////////////////////////////////////////////////
Path Context::GetShaderPath(void) const
{
    return (m_shaderPath);
}

///////////////////////////////////////////////////////////////////////////////
Path Context::GetAssetsPath(void) const
{
    return (m_assetsPath);
}

///////////////////////////////////////////////////////////////////////////////
Path Context::GetScenesPath(void) const
{
    return (m_scenesPath);
}

} // namespace Ray

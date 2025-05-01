///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Context.hpp"
#include "Utils/FileSystem.hpp"

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
void Context::Initialize(void)
{
    RAY_TRACE("Initializing Context...");
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

} // namespace Ray

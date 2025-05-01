///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Raytracer.hpp"
#include "Utils/FileSystem.hpp"
#include "Core/Context.hpp"
#include "Utils/OpenGL.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
const Path Raytracer::PLUGIN_DIRECTORY = "./Plugins/";

///////////////////////////////////////////////////////////////////////////////
Raytracer::Raytracer(int argc, char *argv[])
{
    RAY_UNUSED(argc);
    RAY_UNUSED(argv);
    Context& ctx = Context::GetInstance();

    RAY_SUCCESS("File Discovery");
    Vector<Path> plugins = Fs::DiscoverFilesByExtensions(
        PLUGIN_DIRECTORY, {".rplugin"}
    );

    RAY_SUCCESS("Loading");
    for (const auto& plugin : plugins)
    {
    RAY_SUCCESS("Loads " << plugin);
        DynamicLibrary lib(plugin);
        IPlugin::Symbol func = lib.GetSymbol<UniquePtr<IPlugin>>(
            "CreatePlugin"
        );

        m_libraries.push_back(std::move(lib));
        m_plugins.push_back(func());
    }

    RAY_SUCCESS("Initialize Context");
    ctx.Initialize();
}

///////////////////////////////////////////////////////////////////////////////
void Raytracer::Run(void)
{
    Context& ctx = Context::GetInstance();

    while (!ctx.shutdown)
    {
        for (const auto& plugin : m_plugins)
        {
            plugin->Update(0.f);
        }

        ctx.renderer->Update(0.f);

        for (const auto& plugin : m_plugins)
        {
            plugin->PreRender();
        }

        ctx.renderer->Render();

        OpenGL::BindFramebuffer(0);
        glViewport(
            0, 0,
            ctx.scene->renderOptions.windowResolution.x,
            ctx.scene->renderOptions.windowResolution.y
        );

        ctx.renderer->Present();

        for (const auto& plugin : m_plugins)
        {
            plugin->Render();
        }

        for (const auto& plugin : m_plugins)
        {
            plugin->PostRender();
        }
    }
}

} // namespace Ray

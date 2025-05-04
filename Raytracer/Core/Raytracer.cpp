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
    bool hasWindowingPlugin = false;
    Context& ctx = Context::GetInstance();

    Vector<Path> plugins = Fs::DiscoverFilesByExtensions(
        PLUGIN_DIRECTORY, {".rplugin"}
    );

    for (const auto& plugin : plugins)
    {
        DynamicLibrary lib(plugin);

        auto getType = lib.GetSymbol<IPlugin::Type>("GetPluginType");

        if (getType() == IPlugin::Type::WINDOWING && hasWindowingPlugin)
        {
            RAY_WARN(
                "Multiple Windowing Plugin detected, " << plugin << " ignored"
            );
            continue;
        }
        else if (getType() ==  IPlugin::Type::WINDOWING)
        {
            hasWindowingPlugin = true;
        }

        IPlugin::Symbol func = lib.GetSymbol<UniquePtr<IPlugin>>(
            "CreatePlugin"
        );

        m_libraries.push_back(std::move(lib));
        m_plugins.push_back(func());
    }

    if (!hasWindowingPlugin)
    {
        throw Exception("Error: No windowing plugin loaded.");
    }

    ctx.Initialize(argc, argv);
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

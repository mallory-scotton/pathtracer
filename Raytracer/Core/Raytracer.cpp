///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Raytracer.hpp"
#include "Utils/FileSystem.hpp"
#include "Errors/PluginException.hpp"
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

        IPlugin::Type type = getType();

        if (type == IPlugin::Type::WINDOWING && hasWindowingPlugin)
        {
            RAY_WARN(
                "Multiple Windowing Plugin detected, " << plugin << " ignored"
            );
            continue;
        }
        else if (type ==  IPlugin::Type::WINDOWING)
        {
            hasWindowingPlugin = true;
        }
        else if (type == IPlugin::Type::GUI)
        {
            ctx.hasGui = true;
        }

        IPlugin::Symbol func = lib.GetSymbol<UniquePtr<IPlugin>>(
            "CreatePlugin"
        );

        m_libraries.push_back(std::move(lib));
        m_plugins.push_back(func());
    }

    if (!hasWindowingPlugin)
    {
        throw PluginException("No windowing plugin loaded.");
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
        OpenGL::Viewport(Vec2i(0), ctx.scene->renderOptions.windowResolution);

        ctx.renderer->Present();

        for (const auto& plugin : m_plugins)
        {
            plugin->Render();
        }

        for (const auto& plugin : m_plugins)
        {
            plugin->PostRender();
        }

        if (!ctx.hasGui && ctx.renderer->GetSampleCount() == 200)
        {
            ctx.renderer->ExportRender("out.ppm");
        }
    }
}

} // namespace Ray

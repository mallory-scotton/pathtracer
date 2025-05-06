///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Plugins/SFML/SFMLPlugin.hpp"
#include "Core/Context.hpp"
#include "Utils/OpenGL.hpp"
#include "Errors/Exception.hpp"
#include "ImGui/imgui.h"
#include "ImGui/imgui-SFML.h"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
const sf::ContextSettings SFMLPlugin::WINDOW_CONTEXT(
    24,                             //<! depthBits
    8,                              //<! stencilBits
    0,                              //<! antialiasingLevel
    3,                              //<! majorVersion
    2,                              //<! minorVersion
    sf::ContextSettings::Debug,     //<! attributeFlags
    false                           //<! sRgbCapable
);

///////////////////////////////////////////////////////////////////////////////
SFMLPlugin::SFMLPlugin(void)
    : m_window(
        sf::VideoMode({1280, 720}), "Raytracer",
        sf::Style::Default, WINDOW_CONTEXT
    )
    , m_useImGui(true)
{
    Context& ctx = Context::GetInstance();

    m_window.setActive(true);
    m_window.setFramerateLimit(60);

    m_window.setSize(sf::Vector2u(
        ctx.scene->renderOptions.windowResolution.x,
        ctx.scene->renderOptions.windowResolution.y
    ));

    if (!ImGui::SFML::Init(m_window))
    {
        m_useImGui = false;
    }

    m_previousSize = m_window.getSize();
}

///////////////////////////////////////////////////////////////////////////////
SFMLPlugin::~SFMLPlugin()
{
    if (m_window.isOpen())
    {
        m_window.close();
    }

    if (m_useImGui)
    {
        ImGui::SFML::Shutdown();
    }
}

///////////////////////////////////////////////////////////////////////////////
void SFMLPlugin::Update(float deltaSeconds)
{
    RAY_UNUSED(deltaSeconds);
    Context& ctx = Context::GetInstance();
    sf::Event event;

    if (m_previousSize != m_window.getSize())
    {
        Renderer::Options& options = ctx.scene->renderOptions;
        sf::Vector2u size = m_window.getSize();

        Vec2i newSize(
            static_cast<int>(size.x), static_cast<int>(size.y)
        );

        options.windowResolution = newSize;

        if (!options.independentRenderSize)
        {
            options.renderResolution = options.windowResolution;
        }

        ctx.renderer->ResizeRenderer();

        m_previousSize = size;
    }

    while (m_window.pollEvent(event))
    {
        if (m_useImGui)
        {
            ImGui::SFML::ProcessEvent(m_window, event);
        }

        if (event.type == sf::Event::Closed)
        {
            m_window.close();
            ctx.Shutdown();
            return;
        }
    }

    if (m_useImGui)
    {
        ImGui::SFML::Update(m_window, m_clock.restart());
    }
}

///////////////////////////////////////////////////////////////////////////////
void SFMLPlugin::PreRender(void)
{
    OpenGL::Clear(Vec4f(0.f, 0.f, 0.f, 0.f));
    OpenGL::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    OpenGL::Disable(GL_DEPTH_TEST);
}

///////////////////////////////////////////////////////////////////////////////
void SFMLPlugin::Render(void)
{
    if (m_useImGui)
    {
        ImGui::SFML::Render(m_window);
    }
}

///////////////////////////////////////////////////////////////////////////////
void SFMLPlugin::PostRender(void)
{
    m_window.display();
}

} // namespace Ray

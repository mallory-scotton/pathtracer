///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Plugins/SFML/SFMLPlugin.hpp"
#include "Core/Context.hpp"
#include "Errors/Exception.hpp"

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
{
    Context& ctx = Context::GetInstance();

    m_window.setActive(true);

    m_window.setSize(sf::Vector2u(
        ctx.scene->renderOptions.windowResolution.x,
        ctx.scene->renderOptions.windowResolution.y
    ));

    int initialization = gl3wInit();

    if (initialization != 0)
    {
        throw Exception("Failed to initialize OpenGL loader");
    }

    // if (gl3wIsSupported(3, 0)) {
    //     GLint status = GL_FALSE;
    //     glGetIntegerv(GL_FRAMEBUFFER_SRGB, &status);
    //     if (status == GL_TRUE || gl3wIsSupported(3,0))
    //     {
    //         glDisable(GL_FRAMEBUFFER_SRGB);
    //     }
    // }
}

///////////////////////////////////////////////////////////////////////////////
SFMLPlugin::~SFMLPlugin()
{
    if (m_window.isOpen())
    {
        m_window.close();
    }
}

///////////////////////////////////////////////////////////////////////////////
void SFMLPlugin::Update(float deltaSeconds)
{
    RAY_UNUSED(deltaSeconds);
    Context& ctx = Context::GetInstance();
    sf::Event event;

    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            m_window.close();
            ctx.Shutdown();
        }
        else if (event.type == sf::Event::Resized)
        {
            RenderOptions& options = ctx.scene->renderOptions;
            sf::Vector2u size = m_window.getSize();

            Vec2i newSize(
                static_cast<int>(size.x), static_cast<int>(size.y)
            );

            if (
                options.windowResolution.x == newSize.x &&
                options.windowResolution.y == newSize.y
            )
            {
                continue;
            }

            options.windowResolution = newSize;

            if (!options.independentRenderSize)
            {
                options.renderResolution.x = options.windowResolution.x;
                options.renderResolution.y = options.windowResolution.y;
            }

            ctx.renderer->ResizeRenderer();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void SFMLPlugin::PreRender(void)
{
    glClearColor(0., 0., 0., 0.);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
}

///////////////////////////////////////////////////////////////////////////////
void SFMLPlugin::Render(void)
{}

///////////////////////////////////////////////////////////////////////////////
void SFMLPlugin::PostRender(void)
{
    m_window.display();
}

} // namespace Ray

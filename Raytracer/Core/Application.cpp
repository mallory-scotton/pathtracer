///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Application.hpp"
#include "Utils/Exception.hpp"
#include "Resources/Shader.hpp"
#include "Renderer/Quad.hpp"
#include "Utils/OpenGL.hpp"
#include <random>
#include <GL/glew.h>

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
Application::Application(const Vector<String>& args)
{
    RAY_UNUSED(args);

    std::srand(static_cast<Uint32>(std::time(nullptr)));

    mWindow.reset(new Window());

    if (glewInit() != GLEW_OK)
    {
        throw Exception("Failed to initialize GLEW");
    }

    mScene.reset(new Scene("Scenes/Hyperion.scene"));
    mRenderer.reset(new Renderer(mScene));
}

///////////////////////////////////////////////////////////////////////////////
Application::~Application()
{
    mWindow.reset();
}

///////////////////////////////////////////////////////////////////////////////
void Application::Run(void)
{
    sf::Clock clock;
    float deltaSeconds;

    while (mWindow->IsOpen())
    {
        deltaSeconds = clock.restart().asSeconds();

        mWindow->Update();
        mRenderer->Update(deltaSeconds);

        glDisable(GL_DEPTH_TEST);
        mRenderer->Render();
        mRenderer->DrawToScreen();

        mWindow->Display();
    }
}

} // namespace Ray

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Application.hpp"
#include "Utils/Exception.hpp"
#include "Resources/Shader.hpp"
#include "Renderer/Quad.hpp"
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
}

///////////////////////////////////////////////////////////////////////////////
Application::~Application()
{
    mWindow.reset();
}

///////////////////////////////////////////////////////////////////////////////
void Application::Run(void)
{
    Shader shader(
        "Raytracer/Shaders/Tests/Vertex.glsl",
        "Raytracer/Shaders/Tests/Fragment.glsl"
    );

    Quad view;

    while (mWindow->IsOpen())
    {
        mWindow->Update();

        shader.Use();
        view.Draw();
        shader.StopUsing();

        mWindow->Display();
    }
}

} // namespace Ray

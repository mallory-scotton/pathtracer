///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Config.hpp"
#include "Core/Window.hpp"
#include "Utils/Exception.hpp"
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#ifndef RAY_NO_IMGUI
    #ifndef IMGUI_DEFINE_MATH_OPERATORS
        #define IMGUI_DEFINE_MATH_OPERATORS
    #endif
    #include <imgui-SFML.h>
    #include <imgui.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
Window::Window(const Vec2u& dimension)
    : mWindow(sf::VideoMode(dimension), "Raytracer", sf::Style::Close)
    , mView(mWindow.getDefaultView())
{
#ifndef RAY_NO_IMGUI
    if (!ImGui::SFML::Init(mWindow))
    {
        throw Exception(RAY_ERROR_IMGUI);
    }
#endif
}

///////////////////////////////////////////////////////////////////////////////
Window::~Window()
{
    if (mWindow.isOpen())
    {
        mWindow.close();
    }

#ifndef RAY_NO_IMGUI
    ImGui::SFML::Shutdown();
#endif
}

///////////////////////////////////////////////////////////////////////////////
void Window::Update(void)
{
    RAY_ASSERT(mWindow.isOpen(), "Cannot update a closed window.");

    while (Optional<sf::Event> event = mWindow.pollEvent())
    {
    #ifndef RAY_NO_IMGUI
        ImGui::SFML::ProcessEvent(mWindow, *event);
    #endif

        if (event->is<sf::Event::Closed>())
        {
            mWindow.close();
            break;
        }
    }

#ifndef RAY_NO_IMGUI
    ImGui::SFML::Update(mWindow, mClock.restart());
#endif

#ifndef RAY_NO_IMGUI
    ImGui::Begin("Hello World!");
    ImGui::Button("Look at this pretty button");
    ImGui::End();
#endif

    mWindow.clear();
}

///////////////////////////////////////////////////////////////////////////////
bool Window::IsOpen(void) const
{
    return (mWindow.isOpen());
}

///////////////////////////////////////////////////////////////////////////////
void Window::Display(void)
{
#ifndef RAY_NO_IMGUI
    ImGui::SFML::Render(mWindow);
#endif

    mWindow.display();
}

} // namespace Ray

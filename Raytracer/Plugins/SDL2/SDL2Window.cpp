///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Plugins/SDL2/SDL2Window.hpp"
#include "Core/Context.hpp"
#include "Utils/OpenGL.hpp"
#include "Errors/PluginException.hpp"
#include "Errors/OpenGLException.hpp"
#include "ImGui/imgui_impl_sdl2.h"
#include "ImGui/imgui_impl_opengl3.h"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
void SDL2Window::WindowDeleter::operator()(SDL_Window* window) const
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

///////////////////////////////////////////////////////////////////////////////
SDL2Window::~SDL2Window(void)
{
    SDL_GL_DeleteContext(m_context);

    if (m_useImGui)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
    }
}

///////////////////////////////////////////////////////////////////////////////
SDL2Window::SDL2Window(const Vec2i& dimension)
    : m_useImGui(false)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER))
    {
        String message = "SDL2 initialization failed: ";
        throw PluginException(message + SDL_GetError());
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);

    m_window.reset(SDL_CreateWindow(
        "Raytracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        dimension.x, dimension.y,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
    ));

    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

    m_context = SDL_GL_CreateContext(m_window.get());

    if (!m_context)
    {
        throw OpenGLException("Failed to initialize GL context.");
    }

    SDL_GL_SetSwapInterval(0);
}

///////////////////////////////////////////////////////////////////////////////
void SDL2Window::Update(void)
{
    Context& ctx = Context::GetInstance();
    SDL_Event event;

    static bool noRetry = false;
    if (ImGui::GetCurrentContext() != nullptr && !m_useImGui && !noRetry)
    {
        const char* glslVersion = "#version 130";

        if (!ImGui_ImplSDL2_InitForOpenGL(m_window.get(), m_context))
        {
            m_useImGui = false;
            noRetry = true;
        }
        else if (!ImGui_ImplOpenGL3_Init(glslVersion))
        {
            ImGui_ImplSDL2_Shutdown();
            m_useImGui = false;
            noRetry = true;
        }
        else
        {
            m_useImGui = true;
        }
    }

    while (SDL_PollEvent(&event))
    {
        if (m_useImGui)
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
        }

        if (event.type == SDL_QUIT)
        {
            ctx.Shutdown();
        }
        else if (event.type == SDL_WINDOWEVENT)
        {
            if (event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                Renderer::Options& options = ctx.scene->renderOptions;

                SDL_GL_GetDrawableSize(m_window.get(),
                    &options.windowResolution.x,
                    &options.windowResolution.y
                );

                if (!options.independentRenderSize)
                {
                    options.renderResolution.x = options.windowResolution.x;
                    options.renderResolution.y = options.windowResolution.y;
                }

                ctx.renderer->ResizeRenderer();
            }
            else if (event.window.event == SDL_WINDOWEVENT_CLOSE)
            {
                if (event.window.windowID == SDL_GetWindowID(m_window.get()))
                {
                    ctx.Shutdown();
                }
            }
        }
    }

    if (m_useImGui)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }
}

///////////////////////////////////////////////////////////////////////////////
void SDL2Window::Clear(void)
{
    OpenGL::Clear(Vec4f(0.f, 0.f, 0.f, 0.f));
    OpenGL::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    OpenGL::Disable(GL_DEPTH_TEST);
}

///////////////////////////////////////////////////////////////////////////////
void SDL2Window::SwapWindow(void)
{
    if (m_useImGui)
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    SDL_GL_SwapWindow(m_window.get());
}

} // namespace Ray

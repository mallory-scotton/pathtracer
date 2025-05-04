///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Plugins/Gui/GuiPlugin.hpp"
#include "Core/Context.hpp"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
const float GuiPlugin::MOUSE_SENSITIVITY = 0.01f;

///////////////////////////////////////////////////////////////////////////////
GuiPlugin::GuiPlugin(void)
{
    IMGUI_CHECKVERSION();

    if (ImGui::GetCurrentContext() == nullptr)
    {
        ImGui::CreateContext();
    }

    ImGui::StyleColorsDark();

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

///////////////////////////////////////////////////////////////////////////////
GuiPlugin::~GuiPlugin()
{}

///////////////////////////////////////////////////////////////////////////////
void GuiPlugin::Update(float deltaSeconds)
{
    RAY_UNUSED(deltaSeconds);
    Context& ctx = Context::GetInstance();

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    if (!ImGui::GetIO().WantCaptureMouse && ImGui::IsAnyMouseDown())
    {
        if (ImGui::IsMouseDown(0))
        {
            ImVec2 mouseDelta = ImGui::GetMouseDragDelta(0, 0);
            ctx.scene->camera->OffsetOrientation(mouseDelta.x, mouseDelta.y);
            ImGui::ResetMouseDragDelta(0);
        }
        else if (ImGui::IsMouseDown(1))
        {
            ImVec2 mouseDelta = ImGui::GetMouseDragDelta(1, 0);
            ctx.scene->camera->SetRadius(MOUSE_SENSITIVITY * mouseDelta.y);
            ImGui::ResetMouseDragDelta(1);
        }
        else if (ImGui::IsMouseDown(2))
        {
            ImVec2 mouseDelta = ImGui::GetMouseDragDelta(2, 0);
            ctx.scene->camera->Strafe(
                MOUSE_SENSITIVITY * mouseDelta.x,
                MOUSE_SENSITIVITY * mouseDelta.y
            );
            ImGui::ResetMouseDragDelta(2);
        }
        ctx.scene->dirty = true;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_W))
    {
        ctx.scene->camera->SetRadius(-1.f);
        ctx.scene->camera->isMoving = true;
        ctx.scene->dirty = true;
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_S))
    {
        ctx.scene->camera->SetRadius(1.f);
        ctx.scene->camera->isMoving = true;
        ctx.scene->dirty = true;
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_A))
    {
        ctx.scene->camera->Strafe(1.f, 0.f);
        ctx.scene->camera->isMoving = true;
        ctx.scene->dirty = true;
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_D))
    {
        ctx.scene->camera->Strafe(-1.f, 0.f);
        ctx.scene->camera->isMoving = true;
        ctx.scene->dirty = true;
    }
}

///////////////////////////////////////////////////////////////////////////////
void GuiPlugin::PreRender(void)
{
    ImGuiIO& io = ImGui::GetIO();
    Context& ctx = Context::GetInstance();

    if (io.DisplaySize.x <= 0.f || io.DisplaySize.y <= 0.f || ctx.shutdown)
    {
        return;
    }

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground;

    ImGui::Begin("DockSpaceHost", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f),
        ImGuiDockNodeFlags_PassthruCentralNode
    );

    ImGui::End();

    ImGui::Begin("Settings");
    ImGui::Text("Samples: %d ", ctx.renderer->GetSampleCount());
    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void GuiPlugin::Render(void)
{}

///////////////////////////////////////////////////////////////////////////////
void GuiPlugin::PostRender(void)
{}

} // namespace Ray

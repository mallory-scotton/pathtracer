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
    : m_objectMode(false)
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

    if (m_objectMode)
    {
        ctx.scene->dirty = true;
    }
}

///////////////////////////////////////////////////////////////////////////////
void GuiPlugin::PreRender(void)
{
    ImGuiIO& io = ImGui::GetIO();
    Context& ctx = Context::GetInstance();
    Renderer::Options& options = ctx.scene->renderOptions;

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
    ImGui::Checkbox("Object Mode", &m_objectMode);

    bool optionsChanged = false;
    bool reloadShaders = false;

    if (ImGui::CollapsingHeader("Render Settings"))
    {
        optionsChanged |= ImGui::SliderInt("Max Spp", &options.maxSpp, -1, 256);
        optionsChanged |= ImGui::SliderInt("Max Depth", &options.maxDepth, 1, 10);
        reloadShaders |= ImGui::Checkbox("Enable Russian Roulette", &options.enableRR);
        reloadShaders |= ImGui::SliderInt("Russian Roulette Depth", &options.RRDepth, 1, 10);
        reloadShaders |= ImGui::Checkbox("Enable Roughness Mollification", &options.enableRoughnessMollification);
        optionsChanged |= ImGui::SliderFloat("Roughness Mollification Amount", &options.roughnessMollificationAmt, 0, 1);
        reloadShaders |= ImGui::Checkbox("Enable Volume MIS", &options.enableVolumeMIS);
    }

    if (ImGui::CollapsingHeader("Environment"))
    {
        reloadShaders |= ImGui::Checkbox("Enable Uniform Light", &options.enableUniformLight);

        Vec3f uniformLightCol = Vec3f::Pow(options.uniformLightCol, 1.0 / 2.2);
        optionsChanged |= ImGui::ColorEdit3("Uniform Light Color (Gamma Corrected)", (float*)(&uniformLightCol), 0);
        options.uniformLightCol = Vec3f::Pow(uniformLightCol, 2.2);

        reloadShaders |= ImGui::Checkbox("Enable Environment Map", &options.enableEnvMap);
        optionsChanged |= ImGui::SliderFloat("Enviornment Map Intensity", &options.envMapIntensity, 0.1f, 10.0f);
        optionsChanged |= ImGui::SliderFloat("Enviornment Map Rotation", &options.envMapRot, 0.0f, 360.0f);
        reloadShaders |= ImGui::Checkbox("Hide Emitters", &options.hideEmitters);
        reloadShaders |= ImGui::Checkbox("Enable Background", &options.enableBackground);
        optionsChanged |= ImGui::ColorEdit3("Background Color", (float*)&options.backgroundCol, 0);
        reloadShaders |= ImGui::Checkbox("Transparent Background", &options.transparentBackground);
    }

    if (ImGui::CollapsingHeader("Tonemapping"))
    {
        ImGui::Checkbox("Enable Tonemap", &options.enableTonemap);

        if (options.enableTonemap)
        {
            ImGui::Checkbox("Enable ACES", &options.enableAces);
            if (options.enableAces)
            {
                ImGui::Checkbox("Simple ACES Fit", &options.simpleAcesFit);
            }
        }
    }

    if (ImGui::CollapsingHeader("Denoiser"))
    {
        ImGui::Checkbox("Enable Denoiser", &options.enableDenoiser);
        ImGui::SliderInt("Number of Frames to skip", &options.denoiserFrameCnt, 5, 50);
    }

    if (ImGui::CollapsingHeader("Camera"))
    {
        float fov = Math::Degrees(ctx.scene->camera->fov);
        float aperture = ctx.scene->camera->aperture * 1000.0f;
        optionsChanged |= ImGui::SliderFloat("Fov", &fov, 10, 90);
        ctx.scene->camera->SetFov(fov);
        optionsChanged |= ImGui::SliderFloat("Aperture", &aperture, 0.0f, 10.8f);
        ctx.scene->camera->aperture = aperture / 1000.0f;
        optionsChanged |= ImGui::SliderFloat("Focal Distance", &ctx.scene->camera->focalDist, 0.01f, 50.0f);
        ImGui::Text("Pos: %.2f, %.2f, %.2f", ctx.scene->camera->position.x, ctx.scene->camera->position.y, ctx.scene->camera->position.z);
    }

    if (optionsChanged)
    {
        ctx.scene->dirty = true;
    }

    if (reloadShaders)
    {
        ctx.scene->dirty = true;
        ctx.renderer->ReloadShaders();
    }

    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void GuiPlugin::Render(void)
{}

///////////////////////////////////////////////////////////////////////////////
void GuiPlugin::PostRender(void)
{}

} // namespace Ray

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Plugins/Gui/GuiPlugin.hpp"
#include "Core/Context.hpp"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "Maths/Utils.hpp"

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
    , m_isViewportImageHovered(false)
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

    if (m_isViewportImageHovered)
    {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            ImVec2 mouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, 0.0f);
            if (ctx.scene && ctx.scene->camera)
            {
                ctx.scene->camera->OffsetOrientation(mouseDelta.x, mouseDelta.y);
                ctx.scene->dirty = true;
            }
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
        }
        else if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
        {
            ImVec2 mouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right, 0.0f);
            if (ctx.scene && ctx.scene->camera)
            {
                ctx.scene->camera->SetRadius(MOUSE_SENSITIVITY * mouseDelta.y);
                ctx.scene->dirty = true;
            }
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
        }
        else if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
        {
            ImVec2 mouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle, 0.0f);
            if (ctx.scene && ctx.scene->camera)
            {
                ctx.scene->camera->Strafe(
                    MOUSE_SENSITIVITY * mouseDelta.x,
                    MOUSE_SENSITIVITY * mouseDelta.y
                );
                ctx.scene->dirty = true;
            }
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Middle);
        }
    }

    if (m_isViewportImageHovered && ctx.scene && ctx.scene->camera)
    {
        bool camera_moved_by_keyboard = false;
        float move_speed_factor = 0.1f;

        if (ImGui::IsKeyDown(ImGuiKey_W))
        {
            ctx.scene->camera->SetRadius(-move_speed_factor);
            camera_moved_by_keyboard = true;
        }
        if (ImGui::IsKeyDown(ImGuiKey_S))
        {
            ctx.scene->camera->SetRadius(move_speed_factor);
            camera_moved_by_keyboard = true;
        }
        if (ImGui::IsKeyDown(ImGuiKey_A))
        {
            ctx.scene->camera->Strafe(move_speed_factor, 0.f);
            camera_moved_by_keyboard = true;
        }
        if (ImGui::IsKeyDown(ImGuiKey_D))
        {
            ctx.scene->camera->Strafe(-move_speed_factor, 0.f);
            camera_moved_by_keyboard = true;
        }
        if (ImGui::IsKeyDown(ImGuiKey_E))
        {
            ctx.scene->camera->Strafe(0.f, -move_speed_factor);
            camera_moved_by_keyboard = true;
        }
        if (ImGui::IsKeyDown(ImGuiKey_Q))
        {
            ctx.scene->camera->Strafe(0.f, move_speed_factor);
            camera_moved_by_keyboard = true;
        }

        if (camera_moved_by_keyboard)
        {
            ctx.scene->camera->isMoving = true;
            ctx.scene->dirty = true;
        }
    }

    if (m_objectMode && ctx.scene)
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

    static bool dockspace_initialized = false;
    if (!dockspace_initialized)
    {
        dockspace_initialized = true;

        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

        ImGuiID dock_main_id = dockspace_id;
        ImGuiID dock_id_left;
        ImGuiID dock_id_center;

        ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.25f, &dock_id_left, &dock_id_center);

        ImGui::DockBuilderDockWindow("Settings", dock_id_left);
        ImGui::DockBuilderDockWindow("Viewport", dock_id_center);

        ImGui::DockBuilderFinish(dock_main_id);
    }

    ImGui::End();

    ImGui::Begin("Settings");
    ImGui::Text("Samples: %d ", ctx.renderer->GetSampleCount());
    ImGui::Checkbox("Object Mode", &m_objectMode);

    if (ImGui::Button("Export Render"))
    {
        ctx.renderer->ExportRender("out.ppm");
    }

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

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport", nullptr,
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse
    );
    ImGui::PopStyleVar();

    GLuint textureID = ctx.renderTextureID;
    if (textureID != 0)
    {
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        float imageWidth = static_cast<float>(ctx.scene->renderOptions.renderResolution.x);
        float imageHeight = static_cast<float>(ctx.scene->renderOptions.renderResolution.y);

        float aspect = imageWidth / imageHeight;
        float panelWidth = viewportPanelSize.x;
        float panelHeight = viewportPanelSize.y;
        float panelAspect = panelWidth / panelHeight;

        ImVec2 displaySize;
        if (aspect > panelAspect)
        {
            displaySize.x = panelWidth;
            displaySize.y = panelWidth / aspect;
        } else
        {
            displaySize.y = panelHeight;
            displaySize.x = panelHeight * aspect;
        }

        ImVec2 cursorPos = ImGui::GetCursorPos();
        cursorPos.x += (panelWidth - displaySize.x) * 0.5f;
        cursorPos.y += (panelHeight - displaySize.y) * 0.5f;
        ImGui::SetCursorPos(cursorPos);

        ImVec2 uv0 = ImVec2(0.0f, 1.0f);
        ImVec2 uv1 = ImVec2(1.0f, 0.0f);

        ImGui::Image(static_cast<ImTextureID>(textureID), displaySize, uv0, uv1);

        m_isViewportImageHovered = ImGui::IsItemHovered();
    }
    else
    {
        ImGui::Text("Renderer output not yet available.");
        m_isViewportImageHovered = false;
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

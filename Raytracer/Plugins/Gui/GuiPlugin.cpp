///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Plugins/Gui/GuiPlugin.hpp"
#include "Loaders/Loader.hpp"
#include "Core/Context.hpp"
#include "ImGui/imgui_internal.h"
#include "ImGui/ImGuiFileDialog.h"
#include "Maths/Utils.hpp"
#include "ImGui/ImGuizmo.h"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
const float GuiPlugin::MOUSE_SENSITIVITY = 0.01f;

///////////////////////////////////////////////////////////////////////////////
void GuiPlugin::SetupImGuiStyle(void)
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);

    style.WindowRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.GrabRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.TabRounding = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
GuiPlugin::GuiPlugin(void)
    : m_objectMode(false)
    , m_isViewportImageHovered(false)
    , m_selectedInstance(-1)
    , m_guizmoOperation(ImGuizmo::TRANSLATE)
    , m_guizmoMode(ImGuizmo::WORLD)
    , m_reloadShaders(false)
    , m_instanceHasChanged(false)
    , m_optionsChanged(false)
{
    IMGUI_CHECKVERSION();

    if (ImGui::GetCurrentContext() == nullptr)
    {
        ImGui::CreateContext();
    }

    SetupImGuiStyle();

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

    if (m_isViewportImageHovered && ctx.scene->camera)
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
void GuiPlugin::PrepareDocking(void)
{
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

        ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id,
            ImGuiDir_Left, 0.25f, nullptr, &dockspace_id);
        ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id,
            ImGuiDir_Right, 0.25f, nullptr, &dockspace_id);

        ImGuiID dock_id_right_top = ImGui::DockBuilderSplitNode(dock_id_right,
            ImGuiDir_Up, 0.5f, nullptr, &dock_id_right);

        ImGui::DockBuilderDockWindow("Settings", dock_id_left);
        ImGui::DockBuilderDockWindow("Viewport", dockspace_id);
        ImGui::DockBuilderDockWindow("Scene Hierarchy", dock_id_right_top);
        ImGui::DockBuilderDockWindow("Mesh Inspector", dock_id_right);

        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void GuiPlugin::DrawMainMenuBar(void)
{
    Context& ctx = Context::GetInstance();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::MenuItem("New");
            if (ImGui::MenuItem("Open", "CTRL+O"))
            {
                IGFD::FileDialogConfig config;
                config.path = ".";
                ImGuiFileDialog::Instance()->OpenDialog(
                    "ChooseSceneFile", "Choose File", ".scene", config
                );
            }
            ImGui::Separator();
            ImGui::MenuItem("Save", "CTRL+S");
            if (ImGui::BeginMenu("Export As..."))
            {
                ImGui::MenuItem("Portable Pixmap Format (.ppm)");
                ImGui::MenuItem("Portable Network Graphics (.png)");
                ImGui::MenuItem("Joint Photographic Group (.jpg)");
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "ALT+F4"))
            {
                ctx.Shutdown();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

///////////////////////////////////////////////////////////////////////////////
void GuiPlugin::DrawSettings(void)
{
    Context& ctx = Context::GetInstance();
    Renderer::Options& options = ctx.scene->renderOptions;

    ImGui::Begin("Settings");
    ImGui::Text("Samples: %d ", ctx.renderer->GetSampleCount());
    ImGui::Checkbox("Object Mode", &m_objectMode);

    if (ImGui::Button("Export Render"))
    {
        ctx.renderer->ExportRender("out.ppm");
    }

    if (ImGui::CollapsingHeader("Render Settings"))
    {
        m_optionsChanged |= ImGui::SliderInt("Max Spp", &options.maxSpp, -1, 256);
        m_optionsChanged |= ImGui::SliderInt("Max Depth", &options.maxDepth, 1, 10);
        m_reloadShaders |= ImGui::Checkbox("Enable Russian Roulette", &options.enableRR);
        m_reloadShaders |= ImGui::SliderInt("Russian Roulette Depth", &options.RRDepth, 1, 10);
        m_reloadShaders |= ImGui::Checkbox("Enable Roughness Mollification", &options.enableRoughnessMollification);
        m_optionsChanged |= ImGui::SliderFloat("Roughness Mollification Amount", &options.roughnessMollificationAmt, 0, 1);
        m_reloadShaders |= ImGui::Checkbox("Enable Volume MIS", &options.enableVolumeMIS);
    }

    if (ImGui::CollapsingHeader("Environment"))
    {
        m_reloadShaders |= ImGui::Checkbox("Enable Uniform Light", &options.enableUniformLight);

        Vec3f uniformLightCol = Vec3f::Pow(options.uniformLightCol, 1.0 / 2.2);
        m_optionsChanged |= ImGui::ColorEdit3("Uniform Light Color (Gamma Corrected)", (float*)(&uniformLightCol), 0);
        options.uniformLightCol = Vec3f::Pow(uniformLightCol, 2.2);

        m_reloadShaders |= ImGui::Checkbox("Enable Environment Map", &options.enableEnvMap);
        m_optionsChanged |= ImGui::SliderFloat("Enviornment Map Intensity", &options.envMapIntensity, 0.1f, 10.0f);
        m_optionsChanged |= ImGui::SliderFloat("Enviornment Map Rotation", &options.envMapRot, 0.0f, 360.0f);
        m_reloadShaders |= ImGui::Checkbox("Hide Emitters", &options.hideEmitters);
        m_reloadShaders |= ImGui::Checkbox("Enable Background", &options.enableBackground);
        m_optionsChanged |= ImGui::ColorEdit3("Background Color", (float*)&options.backgroundCol, 0);
        m_reloadShaders |= ImGui::Checkbox("Transparent Background", &options.transparentBackground);
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
        m_optionsChanged |= ImGui::SliderFloat("Fov", &fov, 10, 90);
        ctx.scene->camera->SetFov(fov);
        m_optionsChanged |= ImGui::SliderFloat("Aperture", &aperture, 0.0f, 10.8f);
        ctx.scene->camera->aperture = aperture / 1000.0f;
        m_optionsChanged |= ImGui::SliderFloat("Focal Distance", &ctx.scene->camera->focalDist, 0.01f, 50.0f);
        ImGui::Text("Pos: %.2f, %.2f, %.2f", ctx.scene->camera->position.x, ctx.scene->camera->position.y, ctx.scene->camera->position.z);
    }

    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void GuiPlugin::DrawViewport(void)
{
    Context& ctx = Context::GetInstance();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport", nullptr,
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse
    );
    ImGui::PopStyleVar();

    ImVec2 viewportSize = ImGui::GetContentRegionAvail();

    if (
        viewportSize.x != m_lastViewportSize.x ||
        viewportSize.y != m_lastViewportSize.y ||
        viewportSize.x != ctx.scene->renderOptions.renderResolution.x ||
        viewportSize.y != ctx.scene->renderOptions.renderResolution.y
    )
    {
        m_lastViewportSize = viewportSize;
        ctx.scene->renderOptions.renderResolution.x = (int)viewportSize.x;
        ctx.scene->renderOptions.renderResolution.y = (int)viewportSize.y;
        ctx.scene->dirty = true;
        ctx.renderer->ResizeRenderer();
        ctx.renderer->Update(0.f);
        ctx.renderer->Render();
        ctx.renderer->Present();
    }

    GLuint textureID = ctx.renderTextureID;
    ImVec2 imageScreenPos = ImVec2(0,0);
    ImVec2 imageScreenSize = ImVec2(0,0);

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

        imageScreenPos = ImGui::GetItemRectMin();
        imageScreenSize = ImGui::GetItemRectSize();
    }
    else
    {
        ImGui::Text("Renderer output not yet available.");
        m_isViewportImageHovered = false;
    }

    if (m_selectedInstance != -1)
    {
        float view[16], projection[16];
        ctx.scene->camera->ComputeViewProjectionMatrix(
            view, projection, imageScreenSize.x / imageScreenSize.y
        );

        ImGuizmo::SetRect(imageScreenPos.x, imageScreenPos.y, imageScreenSize.x, imageScreenSize.y);
        if (ImGuizmo::Manipulate(
            view, projection, m_guizmoOperation, m_guizmoMode,
            (float*)&ctx.scene->instances[m_selectedInstance], nullptr, nullptr
        ))
        {
            
        }
    }

    if (ImGuizmo::IsOver())
    {
        m_isViewportImageHovered = false;
    }

    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void GuiPlugin::DrawSceneHierarchy(void)
{
    Context& ctx = Context::GetInstance();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    ImGui::Begin("Scene Hierarchy");
    ImGui::PopStyleVar();

    if (ImGui::BeginListBox("##instances", ImGui::GetContentRegionAvail()))
    {
        for (int i = 0; i < (int)ctx.scene->instances.size(); i++)
        {
            const bool isSelected = (m_selectedInstance == i);
            const Instance& instance = ctx.scene->instances[i];

            if (ImGui::Selectable(
                (instance.name + "##" + std::to_string(i)).c_str(),
                isSelected
            ))
            {
                m_selectedInstance = i;
            }

            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
    }
    ImGui::EndListBox();

    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void GuiPlugin::DrawMeshInspector(void)
{
    Context& ctx = Context::GetInstance();

    ImGui::Begin("Mesh Inspector");

    if (m_selectedInstance != -1 && m_selectedInstance < static_cast<int>(ctx.scene->instances.size()))
    {
        Instance& instance = ctx.scene->instances[m_selectedInstance];
        Mat4x4f& xform = instance.transform;

        if (ImGui::CollapsingHeader("Transformation", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::RadioButton("Translate", m_guizmoOperation == ImGuizmo::TRANSLATE))
            {
                m_guizmoOperation = ImGuizmo::TRANSLATE;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Rotation", m_guizmoOperation == ImGuizmo::ROTATE))
            {
                m_guizmoOperation = ImGuizmo::ROTATE;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Scale", m_guizmoOperation == ImGuizmo::SCALE))
            {
                m_guizmoOperation = ImGuizmo::SCALE;
            }

            if (m_guizmoOperation != ImGuizmo::SCALE)
            {
                if (ImGui::RadioButton("Local", m_guizmoMode == ImGuizmo::LOCAL))
                {
                    m_guizmoMode = ImGuizmo::LOCAL;
                }
                ImGui::SameLine();
                if (ImGui::RadioButton("World", m_guizmoMode == ImGuizmo::WORLD))
                {
                    m_guizmoMode = ImGuizmo::WORLD;
                }
            }

            float translation[3], rotation[3], scale[3];

            ImGuizmo::DecomposeMatrixToComponents((float*)&xform, translation, rotation, scale);

            m_instanceHasChanged |= ImGui::DragFloat3("Translation", translation, 0.1f);
            m_instanceHasChanged |= ImGui::DragFloat3("Rotation", rotation, 1.0f);
            m_instanceHasChanged |= ImGui::DragFloat3("Scale", scale, 0.1f);

            ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, (float*)&xform);
        }

        Material& material = ctx.scene->materials[instance.materialID];

        if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
        {
            Vec3f albedo = Vec3f::Pow(material.baseColor, 1.0f / 2.2f);
            m_instanceHasChanged |= ImGui::ColorEdit3("Albedo (Gamma Corrected)", (float*)&albedo, 0);
            material.baseColor = Vec3f::Pow(albedo, 2.2f);

            m_instanceHasChanged |= ImGui::SliderFloat("Metallic", &material.metallic, 0.f, 1.f);
            m_instanceHasChanged |= ImGui::SliderFloat("Roughness", &material.roughness, 0.001f, 1.0f);
            m_instanceHasChanged |= ImGui::SliderFloat("SpecularTint", &material.specularTint, 0.0f, 1.0f);
            m_instanceHasChanged |= ImGui::SliderFloat("Subsurface", &material.subsurface, 0.0f, 1.0f);
            m_instanceHasChanged |= ImGui::SliderFloat("Anisotropic", &material.anisotropic, 0.0f, 1.0f);
            m_instanceHasChanged |= ImGui::SliderFloat("Sheen", &material.sheen, 0.0f, 1.0f);
            m_instanceHasChanged |= ImGui::SliderFloat("SheenTint", &material.sheenTint, 0.0f, 1.0f);
            m_instanceHasChanged |= ImGui::SliderFloat("Clearcoat", &material.clearcoat, 0.0f, 1.0f);
            m_instanceHasChanged |= ImGui::SliderFloat("ClearcoatGloss", &material.clearcoatGloss, 0.0f, 1.0f);
            m_instanceHasChanged |= ImGui::SliderFloat("SpecTrans", &material.specTrans, 0.0f, 1.0f);
            m_instanceHasChanged |= ImGui::SliderFloat("Ior", &material.ior, 1.001f, 2.0f);

            int mediumType = (int)material.mediumType;
            if (ImGui::Combo("Medium Type", &mediumType, "None\0Absorb\0Scatter\0Emissive\0"))
            {
                m_reloadShaders = true;
                m_instanceHasChanged = true;
                material.mediumType = mediumType;
            }

            if (mediumType != Material::MediumType::NONE)
            {
                Vec3f mediumColor = Vec3f::Pow(material.mediumColor, 1.0 / 2.2);
                m_instanceHasChanged |= ImGui::ColorEdit3("Medium Color (Gamma Corrected)", (float*)(&mediumColor), 0);
                material.mediumColor = Vec3f::Pow(mediumColor, 2.2);

                m_instanceHasChanged |= ImGui::SliderFloat("Medium Density", &material.mediumDensity, 0.0f, 5.0f);

                if(mediumType == Material::MediumType::SCATTER)
                {
                    m_instanceHasChanged |= ImGui::SliderFloat("Medium Anisotropy", &material.mediumAnisotropy, -0.9f, 0.9f);
                }
            }

            int alphaMode = (int)material.alphaMode;
            if (ImGui::Combo("Alpha Mode", &alphaMode, "Opaque\0Blend\0Mask\0"))
            {
                m_reloadShaders = true;
                m_instanceHasChanged = true;
                material.alphaMode = alphaMode;
            }

            if (alphaMode != Material::AlphaMode::OPAQUE)
            {
                m_instanceHasChanged |= ImGui::SliderFloat("Opacity", &material.opacity, 0.0f, 1.0f);
            }
        }
    }

    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void GuiPlugin::DrawFileExplorer(void)
{
    Context& ctx = Context::GetInstance();

    if (ImGuiFileDialog::Instance()->Display(
        "ChooseSceneFile", ImGuiWindowFlags_NoDocking, ImVec2(800, 600))
    )
    {
        ImGui::SetNextWindowFocus();
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            ctx.scene = std::make_unique<Scene>();
            ctx.scene->dirty = true;
            Loader::LoadScene(ImGuiFileDialog::Instance()->GetFilePathName());
            ctx.renderer = std::make_unique<Renderer>();
        }

        ImGuiFileDialog::Instance()->Close();
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

    PrepareDocking();
    DrawMainMenuBar();
    DrawSettings();
    DrawViewport();
    DrawSceneHierarchy();
    DrawMeshInspector();
    DrawFileExplorer();

    if (m_reloadShaders)
    {
        ctx.renderer->ReloadShaders();
    }

    if (m_instanceHasChanged)
    {
        ctx.scene->RebuildInstances();
    }

    if (m_optionsChanged || m_reloadShaders || m_instanceHasChanged)
    {
        ctx.scene->dirty = true;
        ctx.renderer->Update(0.f);
    }

    m_optionsChanged = false;
    m_reloadShaders = false;
    m_instanceHasChanged = false;
}

///////////////////////////////////////////////////////////////////////////////
void GuiPlugin::Render(void)
{}

///////////////////////////////////////////////////////////////////////////////
void GuiPlugin::PostRender(void)
{}

} // namespace Ray

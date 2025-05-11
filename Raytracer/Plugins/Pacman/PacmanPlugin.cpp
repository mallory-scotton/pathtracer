///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Plugins/Pacman/PacmanPlugin.hpp"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/ImGuizmo.h"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
PacmanPlugin::PacmanPlugin(void)
    : m_isPacmanScene(false)
    , m_direction(1.f)
    , m_axisIndex(0)
{}

///////////////////////////////////////////////////////////////////////////////
PacmanPlugin::~PacmanPlugin()
{}

///////////////////////////////////////////////////////////////////////////////
void PacmanPlugin::CheckForPacmanScene(void)
{
    Context& ctx = Context::GetInstance();
    Vector<Instance>& meshes = ctx.scene->instances;

    for (const auto& mesh : meshes)
    {
        if  (mesh.name == "pacman")
        {
            m_isPacmanScene = true;
            return;
        }
    }

    m_isPacmanScene = false;
}

///////////////////////////////////////////////////////////////////////////////
Instance& PacmanPlugin::GetInstance(const String& name) const
{
    Context& ctx = Context::GetInstance();
    Vector<Instance>& meshes = ctx.scene->instances;

    for (auto& mesh : meshes)
    {
        if  (mesh.name == name)
        {
            return (mesh);
        }
    }
    return (ctx.scene->instances[0]);
}

///////////////////////////////////////////////////////////////////////////////
void PacmanPlugin::Update(float deltaSeconds)
{
    Context& ctx = Context::GetInstance();
    CheckForPacmanScene();

    if (!m_isPacmanScene)
    {
        return;
    }

    Instance& pacman = GetInstance("pacman");
    Mat4x4f& xform = pacman.transform;

    if (ImGui::IsKeyDown(ImGuiKey_UpArrow) || ImGui::IsKeyDown(ImGuiKey_DownArrow))
    {
        m_axisIndex = 2;
        m_direction = ImGui::IsKeyDown(ImGuiKey_UpArrow) ? -1.f : 1.f;
    }
    else if (ImGui::IsKeyDown(ImGuiKey_RightArrow) || ImGui::IsKeyDown(ImGuiKey_LeftArrow))
    {
        m_axisIndex = 0;
        m_direction = ImGui::IsKeyDown(ImGuiKey_LeftArrow) ? -1.f : 1.f;
    }
    else
    {
        m_axisIndex = 1;
        m_direction = 0;
    }

    float tr[3], rt[3], sc[3];
    ImGuizmo::DecomposeMatrixToComponents((float*)&xform, tr, rt, sc);

    tr[m_axisIndex] += deltaSeconds * m_direction * 5.f;

    ImGuizmo::RecomposeMatrixFromComponents(tr, rt, sc, (float*)&xform);

    pacman.transform = xform;
    ctx.scene->RebuildInstances();
    ctx.renderer->Update(0.f);
}

///////////////////////////////////////////////////////////////////////////////
void PacmanPlugin::PreRender(void)
{}

///////////////////////////////////////////////////////////////////////////////
void PacmanPlugin::Render(void)
{}

///////////////////////////////////////////////////////////////////////////////
void PacmanPlugin::PostRender(void)
{}

} // namespace Ray

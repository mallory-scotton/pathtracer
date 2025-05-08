///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Builders/MeshInstanceBuilder.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{
///////////////////////////////////////////////////////////////////////////////
MeshInstance MeshInstanceBuilder::Build(void)
{
    return (m_mesh);
}

///////////////////////////////////////////////////////////////////////////////
MeshInstanceBuilder& MeshInstanceBuilder::FromConfiguration(const LibConfig::Setting& config)
{
    Context& ctx = Context::GetInstance();
    String material;
    String file, name;
    Quaternionf rotation;
    Mat4x4f matRot, matTrans, matScale;

    config.Value("name", name);
    config.Value("file", file);

    if (config.Value("material", material))
    {
        int index = ctx.scene->getMaterialID(material);
        if (index == -1)
        {
            index = 0;
        }
        m_mesh.materialID = index;
    }

    if (config.Value("position", m_position))
    {
        matTrans[3][0] = m_position.x;
        matTrans[3][1] = m_position.y;
        matTrans[3][2] = m_position.z;
    }

    if (config.Value("scale", m_scale))
    {
        matScale[0][0] = m_scale.x;
        matScale[1][1] = m_scale.y;
        matScale[2][2] = m_scale.z;
    }

    if (config.Value("rotation", m_rotation))
    {
        matRot = Mat4x4f::QuaternionToMatrix(m_rotation);
    }

    if (!file.empty()){
        m_mesh.meshID = ctx.scene->AddMesh(file);
        if (!name.empty() && name != "none")
        {
            m_mesh.name = name;
        }
        else
        {
            size_t pos = file.find_last_of("/\\");
            m_mesh.name = file.substr(pos + 1);
        }

        m_mesh.transform = matRot * matScale * matTrans;

    }
    return {*this};
}

///////////////////////////////////////////////////////////////////////////////
MeshInstanceBuilder& MeshInstanceBuilder::SetName(const std::string& name)
{
    m_mesh.name = name;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
MeshInstanceBuilder& MeshInstanceBuilder::SetMeshId(int meshId)
{
    m_mesh.meshID = meshId;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
MeshInstanceBuilder& MeshInstanceBuilder::SetMaterialId(int materialId)
{
    m_mesh.materialID = materialId;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
MeshInstanceBuilder& MeshInstanceBuilder::SetTransform(const Mat4x4f& transform)
{
    m_mesh.transform = transform;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
MeshInstanceBuilder& MeshInstanceBuilder::SetPosition(const Vec3f& pos)
{
    m_position = pos;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
MeshInstanceBuilder& MeshInstanceBuilder::SetRotation(const Quaternionf& rotation)
{
    m_rotation = rotation;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
MeshInstanceBuilder& MeshInstanceBuilder::SetScale(const Vec3f& scale)
{
    m_scale = scale;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
bool MeshInstanceBuilder::IDCheck(void)
{
    if (m_mesh.meshID == -1)
    {
        return false;
    }
    return true;
}

} // namespace Ray

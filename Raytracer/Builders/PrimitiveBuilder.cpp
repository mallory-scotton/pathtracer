///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Builders/PrimitiveBuilder.hpp"
#include "Core/Context.hpp"
#include "Objects.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{
///////////////////////////////////////////////////////////////////////////////
Instance PrimitiveBuilder::Build(void)
{
    return (m_instance);
}

///////////////////////////////////////////////////////////////////////////////
PrimitiveBuilder& PrimitiveBuilder::FromConfiguration(
    const LibConfig::Setting& config
)
{
    Context& ctx = Context::GetInstance();
    String material;
    String type, name;
    Quaternionf rotation;
    Mat4x4f matRot, matTrans, matScale;

    config.Value("name", name);
    config.Value("type", type);

    if (config.Value("material", material))
    {
        int index = ctx.scene->getMaterialID(material);
        if (index == -1)
        {
            index = 0;
        }
        m_instance.materialID = index;
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

    if (!type.empty()) {
        m_instance.objectID; // TODO: Add the primitive mesh if not already

        if (!name.empty() && name != "none")
        {
            m_instance.name = name;
        }
        else
        {
            size_t pos = type.find_last_of("/\\");
            m_instance.name = type.substr(pos + 1);
        }

        m_instance.transform = matRot * matScale * matTrans;
    }
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
PrimitiveBuilder& PrimitiveBuilder::SetName(const std::string& name)
{
    m_instance.name = name;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
PrimitiveBuilder& PrimitiveBuilder::SetObjectId(int objectId)
{
    m_instance.objectID = objectId;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
PrimitiveBuilder& PrimitiveBuilder::SetMaterialId(int materialId)
{
    m_instance.materialID = materialId;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
PrimitiveBuilder& PrimitiveBuilder::SetTransform(const Mat4x4f& transform)
{
    m_instance.transform = transform;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
PrimitiveBuilder& PrimitiveBuilder::SetPosition(const Vec3f& pos)
{
    m_position = pos;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
PrimitiveBuilder& PrimitiveBuilder::SetRotation(const Quaternionf& rotation)
{
    m_rotation = rotation;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
PrimitiveBuilder& PrimitiveBuilder::SetScale(const Vec3f& scale)
{
    m_scale = scale;
    return (*this);
}

} // namespace Ray

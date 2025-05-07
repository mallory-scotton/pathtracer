///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Builders/MaterialBuilder.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
Material MaterialBuilder::Build()
{
    return (m_material);
}

///////////////////////////////////////////////////////////////////////////////
void MaterialBuilder::BuildMesh()
{

    Context& ctx = Context::GetInstance();

    if (m_id == -1)
    {
        return;
    }

    MeshInstance instance(m_instanceName, m_id, m_transform, m_materialID);
    ctx.scene->AddMeshInstance(instance);
}

///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::FromConfiguration(const LibConfig::Setting& config)
{

    Context& ctx = Context::GetInstance();
    String material;
    String file, name;
    Vec3f position, scale;
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
        m_materialID = index;
    }

    if (config.Value("position", position))
    {
        matTrans[3][0] = position.x;
        matTrans[3][1] = position.y;
        matTrans[3][2] = position.z;
    }

    if (config.Value("scale", scale))
    {
        matScale[0][0] = scale.x;
        matScale[1][1] = scale.y;
        matScale[2][2] = scale.z;
    }

    if (config.Value("rotation", rotation))
    {
        matRot = Mat4x4f::QuaternionToMatrix(rotation);
    }

    if (!file.empty()){
        m_id = ctx.scene->AddMesh(file);
        if (!name.empty() && name != "none")
        {
            m_instanceName = name;
        }
        else
        {
            size_t pos = file.find_last_of("/\\");
            m_instanceName = file.substr(pos + 1);
        }

        m_transform = matRot * matScale * matTrans;

    }
    return {*this};
}

///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetBaseColor(const Vec3f& color)
{
    m_material.baseColor = color;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetOpacity(float opacity)
{
    m_material.opacity = opacity;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetAlphaMode(Material::AlphaMode mode)
{
    m_material.alphaMode = mode;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetAlphaCutoff(float cutoff)
{
    m_material.alphaCutoff = cutoff;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetEmission(const Vec3f& emission)
{
    m_material.emission = emission;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetMetallic(float metallic)
{
    m_material.metallic = metallic;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetRoughness(float roughness)
{
    m_material.roughness = roughness;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetSubsurface(float subsurface)
{
    m_material.subsurface = subsurface;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetAnisotropic(float anisotropic)
{
    m_material.anisotropic = anisotropic;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetSheen(float sheen)
{
    m_material.sheen = sheen;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetSheenTint(float tint)
{
    m_material.sheenTint = tint;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetClearcoat(float clearcoat)
{
    m_material.clearcoat = clearcoat;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetClearcoatGloss(float gloss)
{
    m_material.clearcoatGloss = gloss;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetSpecTrans(float specTrans)
{
    m_material.specTrans = specTrans;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetIOR(float ior)
{
    m_material.ior = ior;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetMediumType(Material::MediumType type)
{
    m_material.mediumType = type;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetMediumDensity(float density)
{
    m_material.mediumDensity = density;
}

///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetMediumColor(const Vec3f& color)
{
    m_material.mediumColor = color;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetMediumAnisotropy(float anisotropy)
{
    m_material.mediumAnisotropy = anisotropy;
    return (*this);
}

// ///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetAlbedoTexturePath(const Path& path)
{
    // m_material.texid = path;
}

///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetMetallicRoughnessTexturePath(const Path& path)
{

}
///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetNormalTexturePath(const Path& path)
{

}
///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetEmissionTexturePath(const Path& path)
{

}

} // namespace Ray

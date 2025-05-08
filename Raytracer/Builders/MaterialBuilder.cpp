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
MaterialBuilder& MaterialBuilder::FromConfiguration(const LibConfig::Setting& config)
{
    Context& ctx = Context::GetInstance();
    String albedo, metallicRoughness, normal, emission, alpha, medium;

    config.Value("name", m_name);
    config.Value("color", m_material.baseColor);
    config.Value("opacity", m_material.opacity);
    config.Value("alphamode", alpha);
    config.Value("alphacutoff", m_material.alphaCutoff);
    config.Value("emission", m_material.emission);
    config.Value("metallic", m_material.metallic);
    config.Value("roughness", m_material.roughness);
    config.Value("subsurface", m_material.subsurface);
    config.Value("speculartint", m_material.specularTint);
    config.Value("anisotropic", m_material.anisotropic);
    config.Value("sheen", m_material.sheen);
    config.Value("sheentint", m_material.sheenTint);
    config.Value("clearcoat", m_material.clearcoat);
    config.Value("clearcoatgloss", m_material.clearcoatGloss);
    config.Value("spectrans", m_material.specTrans);
    config.Value("ior", m_material.ior);
    config.Value("albedotexture", albedo);
    config.Value("metallicroughnesstexture", metallicRoughness);
    config.Value("normaltexture", normal);
    config.Value("emissiontexture", emission);
    config.Value("mediumtype", medium);
    config.Value("mediumdensity", m_material.mediumDensity);
    config.Value("mediumcolor", m_material.mediumColor);
    config.Value("mediumanisotropy", m_material.mediumAnisotropy);

    if (!albedo.empty() && albedo != "none")
    {
        m_material.baseColorTexId = ctx.scene->AddTexture(albedo);
    }
    if (!metallicRoughness.empty() && metallicRoughness != "none")
    {
        m_material.metallicRoughnessTexID = ctx.scene->AddTexture(metallicRoughness);
    }
    if (!normal.empty() && normal != "none")
    {
        m_material.normalmapTexID = ctx.scene->AddTexture(normal);
    }
    if (!emission.empty() && emission != "none")
    {
        m_material.emissionmapTexID = ctx.scene->AddTexture(emission);
    }

    if (alpha == "opaque")
    {
        m_material.alphaMode = Material::OPAQUE;
    }
    else if (alpha == "blend")
    {
        m_material.alphaMode = Material::BLEND;
    }
    else if (alpha == "mask")
    {
        m_material.alphaMode = Material::MASK;
    }

    if (medium == "absorb")
    {
        m_material.mediumType = Material::ABSORB;
    }
    else if (medium == "scatter")
    {
        m_material.mediumType = Material::SCATTER;
    }
    else if (medium == "emissive")
    {
        m_material.mediumType = Material::EMISSIVE;
    }
    return (*this);
}

///////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetName(const std::string& name)
{
    m_name = name;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////
String MaterialBuilder::GetName(void) const
{
    return (m_name);
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
    Context& ctx = Context::GetInstance();
    m_material.baseColorTexId = ctx.scene->AddTexture(path);
}

///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetMetallicRoughnessTexturePath(const Path& path)
{
    Context& ctx = Context::GetInstance();
    m_material.metallicRoughnessTexID = ctx.scene->AddTexture(path);
}
///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetNormalTexturePath(const Path& path)
{
    Context& ctx = Context::GetInstance();
    m_material.normalmapTexID = ctx.scene->AddTexture(path);
}
///////////////////////////////////////////////////////////////////////////////
MaterialBuilder& MaterialBuilder::SetEmissionTexturePath(const Path& path)
{
    Context& ctx = Context::GetInstance();
    m_material.emissionmapTexID = ctx.scene->AddTexture(path);
}

} // namespace Ray

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Builders/RendererOptionsBuilder.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
Renderer::Options RendererOptionsBuilder::Build(void)
{
    return (m_renderOptions);
}

///////////////////////////////////////////////////////////////////////////////
RendererOptionsBuilder& RendererOptionsBuilder::FromConfiguration(const LibConfig::Setting& config)
{
    Context& ctx = Context::GetInstance();
    String envMap;

    config.Value("envmapfile", envMap);
    config.Value("resolution", m_renderOptions.renderResolution);
    config.Value("windowresolution", m_renderOptions.windowResolution);
    config.Value("envmapintensity", m_renderOptions.envMapIntensity);
    config.Value("maxdepth", m_renderOptions.maxDepth);
    config.Value("maxssp", m_renderOptions.maxSpp);
    config.Value("tilewidth", m_renderOptions.tileWidth);
    config.Value("tileheight", m_renderOptions.tileHeight);
    config.Value("enablerr", m_renderOptions.enableRR);
    config.Value("rrdepth", m_renderOptions.RRDepth);
    config.Value("enabletonemap", m_renderOptions.enableTonemap);
    config.Value("enableaces", m_renderOptions.enableAces);
    config.Value("texarraywidth", m_renderOptions.texArrayWidth);
    config.Value("texarrayheight", m_renderOptions.texArrayHeight);
    config.Value("openglnormalmap", m_renderOptions.openglNormalMap);
    config.Value("hideemitters", m_renderOptions.hideEmitters);
    config.Value("enablebackground", m_renderOptions.enableBackground);
    config.Value("transparentbackground", m_renderOptions.transparentBackground);
    config.Value("backgroundcolor", m_renderOptions.backgroundCol);
    config.Value("independentrendersize", m_renderOptions.independentRenderSize);
    config.Value("envmaprotation", m_renderOptions.envMapRot);
    config.Value("enableroughnessmollification",
        m_renderOptions.enableRoughnessMollification);
    config.Value("roughnessmollificationamt", m_renderOptions.roughnessMollificationAmt);
    config.Value("enablevolumemis", m_renderOptions.enableVolumeMIS);
    config.Value("enableuniformlight", m_renderOptions.enableUniformLight);
    config.Value("uniformlightcolor", m_renderOptions.uniformLightCol);

    if (!envMap.empty() && envMap != "none")
    {
        ctx.scene->AddEnvMap(envMap);
        m_renderOptions.enableEnvMap = true;
    }
    else
    {
        m_renderOptions.enableEnvMap = false;
    }

    if (!m_renderOptions.independentRenderSize)
    {
        m_renderOptions.windowResolution = m_renderOptions.renderResolution;
    }
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
RendererOptionsBuilder& RendererOptionsBuilder::SetWindowResolution(const Vec2i& res)
{
    m_renderOptions.windowResolution = res;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
RendererOptionsBuilder& RendererOptionsBuilder::SetRenderResolution(const Vec2i& res)
{
    m_renderOptions.renderResolution = res;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
RendererOptionsBuilder& RendererOptionsBuilder::SetTileSize(int width, int height)
{
    m_renderOptions.tileWidth = width;
    m_renderOptions.tileHeight = height;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
RendererOptionsBuilder& RendererOptionsBuilder::SetMaxDepth(int depth)
{
    m_renderOptions.maxDepth = depth;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
RendererOptionsBuilder& RendererOptionsBuilder::SetMaxSpp(int spp)
{
    m_renderOptions.maxSpp = spp;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
RendererOptionsBuilder& RendererOptionsBuilder::SetEnvMapPath(const Path& path)
{
    Context& ctx = Context::GetInstance();

    if (!path.empty() && path != "none")
    {
        ctx.scene->AddEnvMap(path);
        m_renderOptions.enableEnvMap = true;
    }
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
RendererOptionsBuilder& RendererOptionsBuilder::SetEnvMapIntensity(float intensity)
{
    m_renderOptions.envMapIntensity = intensity;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
RendererOptionsBuilder& RendererOptionsBuilder::SetEnvMapRotation(float rotation)
{
    m_renderOptions.envMapRot = rotation;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
RendererOptionsBuilder& RendererOptionsBuilder::EnableRR(bool enable, int depth)
{
    m_renderOptions.enableRR = enable;
    m_renderOptions.RRDepth = depth;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
RendererOptionsBuilder& RendererOptionsBuilder::EnableTonemap(bool enable)
{
    m_renderOptions.enableTonemap =enable;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
RendererOptionsBuilder& RendererOptionsBuilder::EnableAces(bool enable)
{
    m_renderOptions.enableAces = enable;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
RendererOptionsBuilder& RendererOptionsBuilder::SetTextureArraySize(int width, int height)
{
    m_renderOptions.texArrayWidth = width;
    m_renderOptions.texArrayHeight = height;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
RendererOptionsBuilder& RendererOptionsBuilder::UseOpenGLNormalMap(bool use)
{
    m_renderOptions.openglNormalMap = use;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
RendererOptionsBuilder& RendererOptionsBuilder::HideEmitters(bool hide)
{
    m_renderOptions.hideEmitters = hide;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
RendererOptionsBuilder& RendererOptionsBuilder::EnableBackground(bool enable)
{
    m_renderOptions.enableBackground = enable;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
RendererOptionsBuilder& RendererOptionsBuilder::SetBackgroundColor(const Vec3f& color)
{
    m_renderOptions.backgroundCol = color;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
RendererOptionsBuilder& RendererOptionsBuilder::UseTransparentBackground(bool use)
{
    m_renderOptions.transparentBackground = use;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
RendererOptionsBuilder& RendererOptionsBuilder::UseIndependentRenderSize(bool use)
{
    m_renderOptions.independentRenderSize = use;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
RendererOptionsBuilder& RendererOptionsBuilder::EnableRoughnessMollification(bool enable, float amount)
{
    m_renderOptions.enableRoughnessMollification = enable;
    m_renderOptions.roughnessMollificationAmt = amount;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
RendererOptionsBuilder& RendererOptionsBuilder::EnableVolumeMIS(bool enable)
{
    m_renderOptions.enableVolumeMIS = enable;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
RendererOptionsBuilder& RendererOptionsBuilder::EnableUniformLight(bool enable, const Vec3f& color)
{
    m_renderOptions.enableUniformLight = enable;
    m_renderOptions.uniformLightCol = color;
    return (*this);
}


} // namespace Ray

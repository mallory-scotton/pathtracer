///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Builders/LightBuilder.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
Light LightBuilder::Build()
{
    return (m_light);
}

///////////////////////////////////////////////////////////////////////////////
LightBuilder& LightBuilder::FromConfiguration(const LibConfig::Setting& config)
{

    Vec3f v1, v2;
    String lightType;

    config.Value("position", m_light.position);
    config.Value("emission", m_light.emission);
    config.Value("radius", m_light.radius);
    config.Value("v1", v1);
    config.Value("v2", v2);
    config.Value("type", lightType);

    if (lightType == "quad")
    {
        m_light.type = Light::RECTANGLE;
        m_light.u = v1 - m_light.position;
        m_light.v = v2 - m_light.position;
        m_light.area = Vec3f::Length(Vec3f::Cross(m_light.u, m_light.v));
    }
    else if (lightType == "sphere")
    {
        m_light.type = Light::SPHERE;
        m_light.area = 4.f * PI * m_light.radius * m_light.radius;
    }
    else if (lightType == "distant")
    {
        m_light.type = Light::DISTANT;
        m_light.area = 0.f;
    }
    return (*this);

}

///////////////////////////////////////////////////////////////////////////////
LightBuilder& LightBuilder::SetType(Light::Type type)
{
    m_light.type = type;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
LightBuilder& LightBuilder::SetPosition(const Vec3f& pos)
{
    m_light.position = pos;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
LightBuilder& LightBuilder::SetEmission(const Vec3f& emission)
{
    m_light.emission = emission;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
LightBuilder& LightBuilder::SetRadius(float radius)
{
    m_light.radius = radius;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
LightBuilder& LightBuilder::SetV1(const Vec3f& v1)
{
    m_light.u = v1;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
LightBuilder& LightBuilder::SetV2(const Vec3f& v2)
{
    m_light.v = v2;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
LightBuilder& LightBuilder::SetArea(float area)
{
    m_light.area = area;
    return (*this);
}


} // namespace Ray

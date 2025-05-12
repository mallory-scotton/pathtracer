///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Objects/Torus.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray::Objects
///////////////////////////////////////////////////////////////////////////////
namespace Ray::Objects
{

///////////////////////////////////////////////////////////////////////////////
const Torus::ConstructorType& Torus::Constructor =
    [](const Optional<LibConfig::Setting>& config) -> UniquePtr<IObject>
{
    float mainRadius = 1.0f;
    float tubeRadius = 0.25f;
    int mainSegments = 32;
    int tubeSegments = 16;

    if (config.has_value())
    {
        config->Value("mainRadius", mainRadius);
        config->Value("tubeRadius", tubeRadius);
        config->Value("mainSegments", mainSegments);
        config->Value("tubeSegments", tubeSegments);
    }

    return (std::make_unique<Objects::Torus>(
        mainRadius, tubeRadius, mainSegments, tubeSegments
    ));
};

///////////////////////////////////////////////////////////////////////////////
Torus::Torus(
    float mainRadius,
    float tubeRadius,
    int mainSegments,
    int tubeSegments
)
    : APrimitiveObject("torus")
    , m_mainRadius(mainRadius)
    , m_tubeRadius(tubeRadius)
    , m_mainSegments(mainSegments)
    , m_tubeSegments(tubeSegments)
{
    if (m_mainSegments < 3)
    {
        m_mainSegments = 3;
    }

    if (m_tubeSegments < 3)
    {
        m_tubeSegments = 3;
    }

    GenerateGeometry();
}

///////////////////////////////////////////////////////////////////////////////
void Torus::GenerateGeometry(void)
{
    m_vertices.clear();
    m_normals.clear();

    for (int i = 0; i < m_mainSegments; ++i)
    {
        float u0 = static_cast<float>(i) /
            m_mainSegments * 2.0f * static_cast<float>(M_PI);
        float u1 = static_cast<float>(i + 1) /
            m_mainSegments * 2.0f * static_cast<float>(M_PI);
        float cos_u0 = cos(u0);
        float sin_u0 = sin(u0);
        float cos_u1 = cos(u1);
        float sin_u1 = sin(u1);

        for (int j = 0; j < m_tubeSegments; ++j)
        {
            float v0 = static_cast<float>(j) /
                m_tubeSegments * 2.0f * static_cast<float>(M_PI);
            float v1 = static_cast<float>(j + 1) /
                m_tubeSegments * 2.0f * static_cast<float>(M_PI);
            float cos_v0 = cos(v0);
            float sin_v0 = sin(v0);
            float cos_v1 = cos(v1);
            float sin_v1 = sin(v1);

            Vec4f p00((m_mainRadius + m_tubeRadius * cos_v0) * cos_u0,
                       m_tubeRadius * sin_v0,
                      (m_mainRadius + m_tubeRadius * cos_v0) * sin_u0, 1.0f);
            Vec4f p10((m_mainRadius + m_tubeRadius * cos_v0) * cos_u1,
                       m_tubeRadius * sin_v0,
                      (m_mainRadius + m_tubeRadius * cos_v0) * sin_u1, 1.0f);
            Vec4f p01((m_mainRadius + m_tubeRadius * cos_v1) * cos_u0,
                       m_tubeRadius * sin_v1,
                      (m_mainRadius + m_tubeRadius * cos_v1) * sin_u0, 1.0f);
            Vec4f p11((m_mainRadius + m_tubeRadius * cos_v1) * cos_u1,
                       m_tubeRadius * sin_v1,
                      (m_mainRadius + m_tubeRadius * cos_v1) * sin_u1, 1.0f);

            Vec4f n00(cos_v0 * cos_u0, sin_v0, cos_v0 * sin_u0, 0.0f);
            Vec4f n10(cos_v0 * cos_u1, sin_v0, cos_v0 * sin_u1, 0.0f);
            Vec4f n01(cos_v1 * cos_u0, sin_v1, cos_v1 * sin_u0, 0.0f);
            Vec4f n11(cos_v1 * cos_u1, sin_v1, cos_v1 * sin_u1, 0.0f);

            m_vertices.push_back(p00); m_normals.push_back(n00);
            m_vertices.push_back(p10); m_normals.push_back(n10);
            m_vertices.push_back(p11); m_normals.push_back(n11);

            m_vertices.push_back(p00); m_normals.push_back(n00);
            m_vertices.push_back(p11); m_normals.push_back(n11);
            m_vertices.push_back(p01); m_normals.push_back(n01);
        }
    }
}

} // namespace Ray::Objects

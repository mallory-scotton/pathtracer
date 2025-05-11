///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Objects/Sphere.hpp"
#include "Maths/Constants.hpp"
#include <cmath>

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray::Objects
///////////////////////////////////////////////////////////////////////////////
namespace Ray::Objects
{

///////////////////////////////////////////////////////////////////////////////
Sphere::Sphere(float radius, int segments, int rings)
    : APrimitiveObject("sphere")
    , m_radius(radius)
    , m_segments(segments)
    , m_rings(rings)
{
    if (m_segments < 3)
    {
        m_segments = 3;
    }

    if (m_rings < 2)
    {
        m_rings = 2;
    }

    GenerateGeometry();
}

///////////////////////////////////////////////////////////////////////////////
void Sphere::GenerateGeometry(void)
{
    m_vertices.clear();
    m_normals.clear();

    if (m_radius <= 0.0f)
    {
        return;
    }

    Ray::Vector<Vec4f> unique_vertices_temp;
    Ray::Vector<Vec4f> unique_normals_temp;

    for (int stack_idx = 0; stack_idx <= m_rings; ++stack_idx)
    {

        float phi = static_cast<float>(stack_idx) * PI /
            static_cast<float>(m_rings);

        float y = m_radius * std::cos(phi);
        float radius_at_stack = m_radius * std::sin(phi);

        for (int sector_idx = 0; sector_idx <= m_segments; ++sector_idx)
        {

            float theta = static_cast<float>(sector_idx) * 2.0f * PI /
                static_cast<float>(m_segments);

            float x = radius_at_stack * std::cos(theta);
            float z = radius_at_stack * std::sin(theta);

            unique_vertices_temp.push_back(Vec4f(x, y, z, 1.0f));

            float normal_x, normal_y, normal_z;
            if (stack_idx == 0)
            {
                normal_x = 0.0f;
                normal_y = 1.0f;
                normal_z = 0.0f;
            }
            else if (stack_idx == m_rings)
            {
                normal_x = 0.0f;
                normal_y = -1.0f;
                normal_z = 0.0f;
            }
            else
            {
                normal_x = x;
                normal_y = y;
                normal_z = z;
    
                float length = std::sqrt(
                    normal_x * normal_x +
                    normal_y * normal_y +
                    normal_z * normal_z
                );
                if (length > 1e-6f) {
                    normal_x /= length;
                    normal_y /= length;
                    normal_z /= length;
                } else {

                    normal_x = 0.0f;
                    normal_y = (y > 0) ? 1.0f : -1.0f;
                    normal_z = 0.0f;
                }
            }

            unique_normals_temp.push_back(
                Vec4f(normal_x, normal_y, normal_z, 0.0f)
            );
        }
    }

    for (int i = 0; i < m_rings; ++i)
    {

        int k1_base = i * (m_segments + 1);

        int k2_base = (i + 1) * (m_segments + 1);

        for (int j = 0; j < m_segments; ++j)
        {

            int v_idx_top_left = k1_base + j;
            int v_idx_bottom_left = k2_base + j;
            int v_idx_bottom_right = k2_base + j + 1;
            int v_idx_top_right = k1_base + j + 1;

            if (i == 0) {
                m_vertices.push_back(unique_vertices_temp[v_idx_top_left]);
                m_normals.push_back(unique_normals_temp[v_idx_top_left]);
                m_vertices.push_back(unique_vertices_temp[v_idx_bottom_left]);
                m_normals.push_back(unique_normals_temp[v_idx_bottom_left]);
                m_vertices.push_back(unique_vertices_temp[v_idx_bottom_right]);
                m_normals.push_back(unique_normals_temp[v_idx_bottom_right]);
            } else if (i == m_rings - 1) {

                m_vertices.push_back(unique_vertices_temp[v_idx_top_left]);
                m_normals.push_back(unique_normals_temp[v_idx_top_left]);
                m_vertices.push_back(unique_vertices_temp[v_idx_bottom_left]);
                m_normals.push_back(unique_normals_temp[v_idx_bottom_left]);
                m_vertices.push_back(unique_vertices_temp[v_idx_top_right]);
                m_normals.push_back(unique_normals_temp[v_idx_top_right]);
            } else {

                m_vertices.push_back(unique_vertices_temp[v_idx_top_left]);
                m_normals.push_back(unique_normals_temp[v_idx_top_left]);
                m_vertices.push_back(unique_vertices_temp[v_idx_bottom_left]);
                m_normals.push_back(unique_normals_temp[v_idx_bottom_left]);
                m_vertices.push_back(unique_vertices_temp[v_idx_bottom_right]);
                m_normals.push_back(unique_normals_temp[v_idx_bottom_right]);

                m_vertices.push_back(unique_vertices_temp[v_idx_top_left]);
                m_normals.push_back(unique_normals_temp[v_idx_top_left]);
                m_vertices.push_back(unique_vertices_temp[v_idx_bottom_right]);
                m_normals.push_back(unique_normals_temp[v_idx_bottom_right]);
                m_vertices.push_back(unique_vertices_temp[v_idx_top_right]);
                m_normals.push_back(unique_normals_temp[v_idx_top_right]);
            }
        }
    }
}

} // namespace Ray::Objects

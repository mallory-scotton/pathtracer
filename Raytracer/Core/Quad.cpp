///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Quad.hpp"
#include "Core/Shader.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
Quad::Quad(void)
    : m_vao(std::make_unique<OpenGL::VertexArray>())
{
    glGenBuffers(1, &m_vbo);
    m_vao->Bind();
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    static const float vertices[] =
    {
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE,
        4 * sizeof(GLfloat), (GLvoid*)0
    );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE,
        4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat))
    );

    m_vao->Unbind();
}

///////////////////////////////////////////////////////////////////////////////
Quad::~Quad()
{
    if (m_vbo != 0)
    {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Quad::Draw(UniquePtr<Shader>& shader)
{
    shader->Use();
    m_vao->Draw(GL_TRIANGLES, 0, 6);
    shader->StopUsing();
}

} // namespace Ray

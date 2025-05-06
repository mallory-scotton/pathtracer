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
const float Quad::VERTICES[] =
{
    -1.0f,  1.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 1.0f,
     1.0f, -1.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 1.0f, 1.0f
};

///////////////////////////////////////////////////////////////////////////////
Quad::Quad(void)
    : m_vao(std::make_unique<OpenGL::VertexArray>())
    , m_vbo(std::make_unique<OpenGL::Buffer>(GL_ARRAY_BUFFER))
{
    m_vao->Bind();
    m_vbo->Bind();

    m_vbo->SetData(sizeof(VERTICES), &VERTICES, GL_STATIC_DRAW);

    m_vao->SetVertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE,
        4 * sizeof(GLfloat), (GLvoid*)0
    );

    m_vao->SetVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE,
        4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat))
    );

    m_vao->Unbind();
}

///////////////////////////////////////////////////////////////////////////////
void Quad::Draw(UniquePtr<Shader>& shader)
{
    shader->Use();
    m_vao->Draw(GL_TRIANGLES, 0, 6);
    shader->StopUsing();
}

} // namespace Ray

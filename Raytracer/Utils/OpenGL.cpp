///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Utils/OpenGL.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
void OpenGL::UseProgram(GLuint program)
{
    glUseProgram(program);
}

///////////////////////////////////////////////////////////////////////////////
void OpenGL::StopUsingProgram(void)
{
    glUseProgram(0);
}

///////////////////////////////////////////////////////////////////////////////
GLuint OpenGL::CreateProgram(void)
{
    return (glCreateProgram());
}

///////////////////////////////////////////////////////////////////////////////
GLuint OpenGL::CreateProgram(GLuint shader)
{
    GLuint object = glCreateProgram();

    OpenGL::LinkProgram(object, shader);

    return (object);
}

///////////////////////////////////////////////////////////////////////////////
GLuint OpenGL::CreateProgram(const Vector<GLuint>& shaders)
{
    GLuint object = glCreateProgram();

    OpenGL::LinkProgram(object, shaders);

    return (object);
}

///////////////////////////////////////////////////////////////////////////////
void OpenGL::DeleteProgram(GLuint& program)
{
    if (program != 0)
    {
        glDeleteProgram(program);
        program = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
void OpenGL::AttachShader(GLuint program, GLuint shader)
{
    glAttachShader(program, shader);
}

///////////////////////////////////////////////////////////////////////////////
void OpenGL::AttachShaders(GLuint program, const Vector<GLuint>& shaders)
{
    for (const auto& shader : shaders)
    {
        glAttachShader(program, shader);
    }
}

///////////////////////////////////////////////////////////////////////////////
void OpenGL::DetachShader(GLuint program, GLuint shader)
{
    glDetachShader(program, shader);
}

///////////////////////////////////////////////////////////////////////////////
void OpenGL::DetachShaders(GLuint program, const Vector<GLuint>& shaders)
{
    for (const auto& shader : shaders)
    {
        glDetachShader(program, shader);
    }
}

///////////////////////////////////////////////////////////////////////////////
void OpenGL::LinkProgram(GLuint program)
{
    glLinkProgram(program);
}

///////////////////////////////////////////////////////////////////////////////
void OpenGL::LinkProgram(GLuint program, GLuint shader)
{
    OpenGL::AttachShader(program, shader);
    glLinkProgram(program);
    OpenGL::DetachShader(program, shader);
}

///////////////////////////////////////////////////////////////////////////////
void OpenGL::LinkProgram(GLuint program, const Vector<GLuint>& shaders)
{
    OpenGL::AttachShaders(program, shaders);
    glLinkProgram(program);
    OpenGL::DetachShaders(program, shaders);
}

///////////////////////////////////////////////////////////////////////////////
GLint OpenGL::GetProgramLinkStatus(GLuint program)
{
    GLint result = 0;

    glGetProgramiv(program, GL_LINK_STATUS, &result);

    return (result);
}

///////////////////////////////////////////////////////////////////////////////
String OpenGL::GetProgramInfoLog(GLuint program)
{
    GLint logSize = 0;
    String info;

    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);

    info.resize(static_cast<Uint64>(logSize));
    glGetProgramInfoLog(program, logSize, NULL, info.data());

    return (info);
}

///////////////////////////////////////////////////////////////////////////////
void OpenGL::DeleteShader(GLuint shader)
{
    if (shader != 0)
    {
        glDeleteShader(shader);
    }
}

///////////////////////////////////////////////////////////////////////////////
void OpenGL::DeleteShaders(const Vector<GLuint>& shaders)
{
    for (const auto& shader : shaders)
    {
        if (shader != 0)
        {
            glDeleteShader(shader);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
GLint OpenGL::GetUniformLocation(GLuint program, const String& uniform)
{
    if (program == 0)
    {
        return (-1);
    }

    return (glGetUniformLocation(program, uniform.c_str()));
}

///////////////////////////////////////////////////////////////////////////////
void OpenGL::Uniform(GLuint location, float a)
{
    if (location != -1)
    {
        glUniform1f(location, a);
    }
}

///////////////////////////////////////////////////////////////////////////////
void OpenGL::Uniform(GLuint location, int a)
{
    if (location != -1)
    {
        glUniform1i(location, a);
    }
}

///////////////////////////////////////////////////////////////////////////////
void OpenGL::Uniform(GLuint location, float a, float b)
{
    if (location != -1)
    {
        glUniform2f(location, a, b);
    }
}

///////////////////////////////////////////////////////////////////////////////
void OpenGL::Uniform(GLuint location, int a, int b)
{
    if (location != -1)
    {
        glUniform2i(location, a, b);
    }
}

///////////////////////////////////////////////////////////////////////////////
void OpenGL::Uniform(GLuint location, float a, float b, float c)
{
    if (location != -1)
    {
        glUniform3f(location, a, b, c);
    }
}

///////////////////////////////////////////////////////////////////////////////
void OpenGL::Uniform(GLuint location, int a, int b, int c)
{
    if (location != -1)
    {
        glUniform3i(location, a, b, c);
    }
}

///////////////////////////////////////////////////////////////////////////////
GLuint OpenGL::CreateShader(GLuint shaderType)
{
    return (glCreateShader(shaderType));
}

///////////////////////////////////////////////////////////////////////////////
GLuint OpenGL::CreateShader(const String& source, GLuint shaderType)
{
    GLuint shader = OpenGL::CreateShader(shaderType);
    OpenGL::ShaderSource(shader, source);
    return (shader);
}

///////////////////////////////////////////////////////////////////////////////
void OpenGL::ShaderSource(GLuint shader, const String& source)
{
    const GLchar* sourceChar = (const GLchar*)source.c_str();
    glShaderSource(shader, 1, &sourceChar, 0);
}

///////////////////////////////////////////////////////////////////////////////
GLint OpenGL::CompileShader(GLuint shader)
{
    glCompileShader(shader);
    return (OpenGL::GetShaderCompileStatus(shader));
}

///////////////////////////////////////////////////////////////////////////////
GLint OpenGL::GetShaderCompileStatus(GLuint shader)
{
    GLint result = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    return (result);
}

///////////////////////////////////////////////////////////////////////////////
String OpenGL::GetShaderInfoLog(GLuint shader)
{
    GLint logSize = 0;
    String info;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);

    info.resize(static_cast<Uint64>(logSize));
    glGetShaderInfoLog(shader, logSize, NULL, info.data());

    return (info);
}

} // namespace Ray

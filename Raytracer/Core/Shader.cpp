///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Shader.hpp"
#include "Errors/ShaderException.hpp"
#include "Utils/OpenGL.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
const Path Shader::INCLUDE_PATH = "Shaders/";

///////////////////////////////////////////////////////////////////////////////
const Regex Shader::INCLUDE_REGEX("#include\\s+[\"<]([^\"<>]+)[\">]");

///////////////////////////////////////////////////////////////////////////////
const Regex Shader::COMMENT_SINGLE_LINE_REGEX("//.*(?:\\r\\n|\\n|$)");

///////////////////////////////////////////////////////////////////////////////
const Regex Shader::COMMENT_MULTI_LINE_REGEX("/\\*[\\s\\S]*?\\*/");

///////////////////////////////////////////////////////////////////////////////
const Regex Shader::MULTIPLE_BLANK_LINE_REGEX("\\n\\s*\\n\\s*\\n+");

///////////////////////////////////////////////////////////////////////////////
Shader::Shader(const String& source, GLuint shaderType)
    : m_object(0)
{
    GLuint shaderObj = CompileShader(source, shaderType);

    CompileProgram({shaderObj});
}

///////////////////////////////////////////////////////////////////////////////
Shader::Shader(const String& vertexSource, const String& fragmentSource)
    : m_object(0)
{
    GLuint vertexObj = CompileShader(vertexSource, GL_VERTEX_SHADER);
    GLuint fragmentObj = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);
    CompileProgram({vertexObj, fragmentObj});
}

///////////////////////////////////////////////////////////////////////////////
Shader::~Shader()
{
    OpenGL::DeleteProgram(m_object);
}

///////////////////////////////////////////////////////////////////////////////
GLuint Shader::GetObject() const
{
    return (m_object);
}

///////////////////////////////////////////////////////////////////////////////
void Shader::Use(void)
{
    OpenGL::UseProgram(m_object);
}

///////////////////////////////////////////////////////////////////////////////
void Shader::StopUsing(void)
{
    OpenGL::StopUsingProgram();
}

///////////////////////////////////////////////////////////////////////////////
void Shader::CompileProgram(const Vector<GLuint>& objects)
{
    OpenGL::DeleteProgram(m_object);

    m_object = OpenGL::CreateProgram(objects);

    OpenGL::DeleteShaders(objects);

    if (OpenGL::GetProgramLinkStatus(m_object) == GL_FALSE)
    {
        String information = OpenGL::GetProgramInfoLog(m_object);
        OpenGL::DeleteProgram(m_object);
        throw ShaderException(RAY_ERROR_SHADER_PROGRAM + information);
    }
}

///////////////////////////////////////////////////////////////////////////////
GLuint Shader::CompileShader(const String& source, GLuint shaderType)
{
    GLuint object = OpenGL::CreateShader(source, shaderType);

    if (OpenGL::CompileShader(object) == GL_FALSE)
    {
        String information = OpenGL::GetShaderInfoLog(object);
        OpenGL::DeleteShader(object);
        throw ShaderException(RAY_ERROR_SHADER_COMPILATION + information);
    }

    return (object);
}

///////////////////////////////////////////////////////////////////////////////
String Shader::ReadFile(const Path& filePath)
{
    IfStream file(filePath);

    if (!file.is_open())
    {
        throw ShaderException(RAY_ERROR_FAILED_OPEN_SHADER + filePath.string());
    }

    StringStream buffer;
    buffer << file.rdbuf();

    return (buffer.str());
}

///////////////////////////////////////////////////////////////////////////////
String Shader::RemoveComments(const String& content)
{
    String result = content;
    std::smatch match;

    while (std::regex_search(result, match, COMMENT_MULTI_LINE_REGEX))
    {
        result.replace(match.position(), match.length(), " ");
    }

    while (std::regex_search(result, match, COMMENT_SINGLE_LINE_REGEX))
    {
        result.replace(match.position(), match.length() - 1, " ");
    }

    while (std::regex_search(result, match, MULTIPLE_BLANK_LINE_REGEX))
    {
        result.replace(match.position(), match.length(), "\n\n");
    }

    return (result);
}

///////////////////////////////////////////////////////////////////////////////
String Shader::ProcessIncludes(
    const String& content,
    UnorderedSet<Path>& includedFiles,
    const Path& currentDir
)
{
    String result = content;
    std::smatch match;

    while (std::regex_search(result, match, INCLUDE_REGEX))
    {
        Path includePath = match[1].str();
        Path fullPath;

        if (includePath.is_relative())
        {
            if (includePath.string().front() != '/')
            {
                fullPath = currentDir / includePath;

                if (!std::filesystem::exists(fullPath))
                {
                    fullPath = INCLUDE_PATH / includePath;
                }
            }
            else
            {
                fullPath = INCLUDE_PATH / includePath.string().substr(1);
            }
        }
        else
        {
            fullPath = includePath;
        }

        String replacement;

        if (includedFiles.find(fullPath) == includedFiles.end())
        {
            includedFiles.insert(fullPath);

            String includedContent = ReadFile(fullPath);
            replacement = ProcessIncludes(
                includedContent, includedFiles, fullPath.parent_path()
            );
        }
        else
        {
            replacement = "";
        }

        result.replace(match.position(), match.length(), replacement);
    }

    return (result);
}

///////////////////////////////////////////////////////////////////////////////
String Shader::Source(const Path& filePath)
{
    RAY_TRACE("Parsing shaders: " << filePath);
    UnorderedSet<Path> includedFiles;
    String content = ReadFile(filePath);

    Path currentDir = filePath.parent_path();

    String processedContent = ProcessIncludes(
        content, includedFiles, currentDir
    );

    return (RemoveComments(processedContent));
}

///////////////////////////////////////////////////////////////////////////////
void Shader::Uniform(const String& uniform, int data)
{
    GLint location = OpenGL::GetUniformLocation(m_object, uniform);
    OpenGL::Uniform(location, data);
}

///////////////////////////////////////////////////////////////////////////////
void Shader::Uniform(const String& uniform, float data)
{
    GLint location = OpenGL::GetUniformLocation(m_object, uniform);
    OpenGL::Uniform(location, data);
}

///////////////////////////////////////////////////////////////////////////////
void Shader::Uniform(const String& uniform, const Vec2f& data)
{
    GLint location = OpenGL::GetUniformLocation(m_object, uniform);
    OpenGL::Uniform(location, data.x, data.y);
}

///////////////////////////////////////////////////////////////////////////////
void Shader::Uniform(const String& uniform, const Vec2i& data)
{
    GLint location = OpenGL::GetUniformLocation(m_object, uniform);
    OpenGL::Uniform(location, data.x, data.y);
}

///////////////////////////////////////////////////////////////////////////////
void Shader::Uniform(const String& uniform, const Vec3f& data)
{
    GLint location = OpenGL::GetUniformLocation(m_object, uniform);
    OpenGL::Uniform(location, data.x, data.y, data.z);
}

///////////////////////////////////////////////////////////////////////////////
void Shader::Uniform(const String& uniform, const Vec3i& data)
{
    GLint location = OpenGL::GetUniformLocation(m_object, uniform);
    OpenGL::Uniform(location, data.x, data.y, data.z);
}

} // namespace Ray

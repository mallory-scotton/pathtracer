///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Shader.hpp"
#include "Errors/Exception.hpp"
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
const Regex Shader::INCLUDE_REGEX("#include\\s+[\"<]([^\"<>]+)[\">]");
const Regex Shader::COMMENT_SINGLE_LINE_REGEX("//.*(?:\\r\\n|\\n|$)");
const Regex Shader::COMMENT_MULTI_LINE_REGEX("/\\*[\\s\\S]*?\\*/");
const Regex Shader::MULTIPLE_BLANK_LINE_REGEX("\\n\\s*\\n\\s*\\n+");

///////////////////////////////////////////////////////////////////////////////
Shader::Shader(const String& source, GLenum shaderType)
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
    if (m_object != 0)
    {
        glDeleteProgram(m_object);
    }
}

///////////////////////////////////////////////////////////////////////////////
GLuint Shader::GetObject() const
{
    return (m_object);
}

///////////////////////////////////////////////////////////////////////////////
void Shader::Use(void)
{
    glUseProgram(m_object);
}

///////////////////////////////////////////////////////////////////////////////
void Shader::StopUsing(void)
{
    glUseProgram(0);
}

///////////////////////////////////////////////////////////////////////////////
void Shader::CompileProgram(const Vector<GLuint>& objects)
{
    if (m_object != 0)
    {
        glDeleteProgram(m_object);
    }

    m_object = glCreateProgram();

    for (const auto& object : objects)
    {
        glAttachShader(m_object, object);
    }

    glLinkProgram(m_object);

    for (const auto& object : objects)
    {
        glDetachShader(m_object, object);
    }

    for (const auto& object : objects)
    {
        glDeleteShader(object);
    }

    GLint success = 0;
    glGetProgramiv(m_object, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        std::string msg("Error while linking program\n");
        GLint logSize = 0;
        glGetProgramiv(m_object, GL_INFO_LOG_LENGTH, &logSize);
        char* info = new char[logSize + 1];
        glGetShaderInfoLog(m_object, logSize, NULL, info);
        msg += info;
        delete[] info;
        glDeleteProgram(m_object);
        m_object = 0;
        RAY_ERROR(RAY_ERROR_SHADER_PROGRAM << "\n" << msg);
        throw std::runtime_error(msg.c_str());
    }
}

///////////////////////////////////////////////////////////////////////////////
GLuint Shader::CompileShader(const String& source, GLuint shaderType)
{
    GLuint object = glCreateShader(shaderType);

    const GLchar* src = (const GLchar*)source.c_str();
    glShaderSource(object, 1, &src, 0);
    glCompileShader(object);

    GLint success = 0;
    glGetShaderiv(object, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE)
    {
        std::string msg;
        GLint logSize = 0;
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &logSize);
        char* info = new char[logSize + 1];
        glGetShaderInfoLog(object, logSize, NULL, info);
        msg += "\n";
        msg += info;
        delete[] info;
        glDeleteShader(object);
        object = 0;
        RAY_ERROR(RAY_ERROR_SHADER_COMPILATION << msg);
        throw Exception(RAY_ERROR_SHADER_COMPILATION + msg);
    }

    return (object);
}

///////////////////////////////////////////////////////////////////////////////
String Shader::ReadFile(const Path& filePath)
{
    IfStream file(filePath);

    if (!file.is_open())
    {
        throw Exception(RAY_ERROR_FAILED_OPEN_SHADER + filePath.string());
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

} // namespace Ray

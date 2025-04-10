///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Resources/Shader.hpp"
#include "Utils/Exception.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
const Path Shader::INCLUDE_PATH = "Raytracer/Shaders/";
const Regex Shader::INCLUDE_REGEX("#include\\s+[\"<]([^\"<>]+)[\">]");
const Regex Shader::COMMENT_SINGLE_LINE_REGEX("//.*(?:\\r\\n|\\n|$)");
const Regex Shader::COMMENT_MULTI_LINE_REGEX("/\\*[\\s\\S]*?\\*/");
const Regex Shader::MULTIPLE_BLANK_LINE_REGEX("\\n\\s*\\n\\s*\\n+");

///////////////////////////////////////////////////////////////////////////////
Shader::Shader(
    const Path& vertexShaderPath,
    const Path& fragmentShaderPath,
    const String& definitions
)
{
    String vertexSource = ParseFromFile(vertexShaderPath);
    String fragmentSource = ParseFromFile(fragmentShaderPath);

    if (!definitions.empty())
    {
        Uint64 idx = fragmentSource.find("#version");

        if (idx != String::npos)
        {
            idx = fragmentSource.find("\n", idx);
        }
        else
        {
            idx = 0;
        }

        fragmentSource.insert(idx + 1, definitions);
    }

    mVertexObject = Compile(vertexSource, GL_VERTEX_SHADER);
    mFragmentObject = Compile(fragmentSource, GL_FRAGMENT_SHADER);

    mProgram = glCreateProgram();

    glAttachShader(mProgram, mVertexObject);
    glAttachShader(mProgram, mFragmentObject);

    glLinkProgram(mProgram);

    glDetachShader(mProgram, mVertexObject);
    glDetachShader(mProgram, mFragmentObject);

    GLint success = 0;
    glGetProgramiv(mProgram, GL_LINK_STATUS, &success);

    if (success == GL_FALSE)
    {
        String message = "Error while linking program\n";
        GLint logSize = 0;
        glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &logSize);
        char* info = new char[logSize + 1];
        glGetShaderInfoLog(mProgram, logSize, NULL, info);
        message += info;
        delete[] info;
        glDeleteProgram(mProgram);
        mProgram = 0;
        printf("Error %s\n", message.c_str());
        throw Exception(message.c_str());
    }
}

///////////////////////////////////////////////////////////////////////////////
Shader::~Shader()
{
    if (mVertexObject != 0)
    {
        glDeleteShader(mVertexObject);
        mVertexObject = 0;
    }

    if (mFragmentObject != 0)
    {
        glDeleteShader(mFragmentObject);
        mFragmentObject = 0;
    }

    if (mProgram != 0)
    {
        glDeleteProgram(mProgram);
        mProgram = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
GLuint Shader::Compile(const String& content, GLenum type)
{
    GLuint object = 0;

    String initStatus = String(
        reinterpret_cast<const char*>(glewGetErrorString(glewInit()))
    );
    if (initStatus != "No error")
    {
        throw Exception("GLEW not initialized before shader compilation");
    }

    object = glCreateShader(type);

    if (object == 0)
    {
        throw Exception("Failed to create shader object");
    }

    const GLchar* source = reinterpret_cast<const GLchar*>(content.c_str());
    glShaderSource(object, 1, &source, 0);
    glCompileShader(object);

    GLint success;
    glGetShaderiv(object, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE)
    {
        String errorMessage;

        GLint logSize = 0;
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &logSize);
        
        char* info = new char[logSize + 1];
        glGetShaderInfoLog(object, logSize, NULL, info);
        info[logSize] = '\0';
        errorMessage += info;
        delete[] info;
        glDeleteShader(object);
        object = 0;

        throw Exception(RAY_ERROR_SHADER_COMPILATION + errorMessage);
    }

    return (object);
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
String Shader::RemoveComments(const String& content)
{
    String result = content;
    std::smatch match;

    while (std::regex_search(result, match, COMMENT_MULTI_LINE_REGEX))
    {
        result.replace(match.position(), match.length() - 1, " ");
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
String Shader::ParseFromFile(const Path& filePath)
{
    UnorderedSet<Path> includedFiles;
    String content = ReadFile(filePath);

    Path currentDir = filePath.parent_path();

    String processedContent = ProcessIncludes(
        content, includedFiles, currentDir
    );

    return (RemoveComments(processedContent));
}

///////////////////////////////////////////////////////////////////////////////
void Shader::Use(void)
{
    glUseProgram(mProgram);
}

///////////////////////////////////////////////////////////////////////////////
void Shader::StopUsing(void)
{
    glUseProgram(0);
}

///////////////////////////////////////////////////////////////////////////////
GLuint Shader::GetProgram(void) const
{
    return (mProgram);
}

} // namespace Ray

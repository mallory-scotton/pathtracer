///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Renderer/Scene.hpp"
#include "Renderer/Renderer.hpp"
#include "Maths/Vec4.hpp"
#include "Utils/Exception.hpp"
#include "Utils/Utils.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
Scene::Scene(const Path& filePath)
{
    if (!ParseSceneFile(filePath))
    {
        throw Exception(RAY_ERROR_SCENE_PARSE);
    }
}

///////////////////////////////////////////////////////////////////////////////
String Scene::ReadFile(const Path& filePath) const
{
    IfStream file(filePath);

    if (!file.is_open())
    {
        throw Exception(RAY_ERROR_SCENE_LOAD + filePath.string());
    }

    String content = String(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );

    file.close();

    return (content);
}

///////////////////////////////////////////////////////////////////////////////
Vector<Scene::Object> Scene::ParseObjectsFromFile(const Path& filePath) const
{
    static const Regex objectExp(R"((\w+)\s*\{([^}]*)\})");
    static const Regex propExp(R"((\w+)\s*:\s*([^;]+);)");
    static const Map<String, Scene::Object::Type> objectTypes =
    {
        {"renderer", Scene::Object::Type::RENDERER},
        {"camera", Scene::Object::Type::CAMERA},
        {"mesh", Scene::Object::Type::MESH},
        {"light", Scene::Object::Type::LIGHT},
        {"material", Scene::Object::Type::MATERIAL}
    };

    Vector<Scene::Object> objects;
    String content = ReadFile(filePath);

    auto objectBegin = std::sregex_iterator(
        content.begin(), content.end(), objectExp
    );
    auto objectEnd = std::sregex_iterator();

    for (std::sregex_iterator i = objectBegin; i != objectEnd; i++)
    {
        std::smatch match = *i;
        Scene::Object object;

        String type = Utils::ToLower(match[1].str());
        String body = match[2].str();

        if (!objectTypes.count(type))
        {
            RAY_WARN("Unknown object types: \"" + type + "\"");
            continue;
        }

        object.type = objectTypes.at(type);

        auto propBegin = std::sregex_iterator(
            body.begin(), body.end(), propExp
        );
        auto propEnd = std::sregex_iterator();

        for (std::sregex_iterator j = propBegin; j != propEnd; j++)
        {
            std::smatch prop = *j;

            String key = prop[1].str();
            String valueStr = prop[2].str();

            Vector<String> values;
            std::istringstream iss(valueStr);
            String value;

            while (iss >> value)
            {
                values.push_back(value);
            }

            object.properties[key] = values;
        }

        objects.push_back(object);
    }

    return (objects);
}

///////////////////////////////////////////////////////////////////////////////
bool Scene::ParseSceneFile(const Path& filePath)
{
    RAY_TRACE("Parsing Scene file...");

    Vector<Scene::Object> objects = ParseObjectsFromFile(filePath);

    for (const auto& object : objects)
    {
        switch (object.type)
        {
            case Object::Type::CAMERA:
            {
                mCamera.reset(new Camera(object.properties));
                break;
            }
            case Object::Type::LIGHT:
            {
                mLights.push_back(Light(object.properties));
                break;
            }
            case Object::Type::MATERIAL:
            {
                String name = "";

                for (const auto& [key, values] : object.properties)
                {
                    if (Utils::Equals(key, "name") && values.size() == 1)
                    {
                        name = values[0];
                    }
                }

                if (name.empty())
                {
                    RAY_WARN("No name defined for Material");
                    break;
                }

                if (mMaterialMap.count(name) != 0)
                {
                    RAY_WARN("Material \"" << name << "\" is already defined");
                    break;
                }

                mMaterials.push_back(Material(object.properties, this));
                mMaterialMap[name] = mMaterials.size() - 1;
                break;
            }
            case Object::Type::MESH:
            {
                break; // TODO: Add mesh loading
            }
            case Object::Type::RENDERER:
            {
                mOptions = Renderer::Options(object.properties);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    RAY_INFO("Lights count: " << mLights.size());
    RAY_INFO("Materials count: " << mMaterials.size());

    return (true);
}

///////////////////////////////////////////////////////////////////////////////
Uint64 Scene::AddTexture(const Path& filePath)
{
    for (Uint64 i = 0; i < mTextures.size(); i++)
    {
        if (mTextures[i]->GetName() == filePath)
        {
            return (i);
        }
    }

    RAY_TRACE("Loading texture " << filePath);
    mTextures.push_back(std::make_unique<Texture>(filePath));

    return (mTextures.size() - 1);
}

///////////////////////////////////////////////////////////////////////////////
bool Scene::IsUsingLights(void) const
{
    return (mLights.size() > 0);
}

///////////////////////////////////////////////////////////////////////////////
bool Scene::IsUsingAlphaTest(void) const
{
    for (const auto& material : mMaterials)
    {
        if (material.GetAlphaMode() != Material::AlphaMode::OPAQUE)
        {
            return (true);
        }
    }

    return (false);
}

///////////////////////////////////////////////////////////////////////////////
bool Scene::IsUsingMedium(void) const
{
    for (const auto& material : mMaterials)
    {
        if (material.GetMediumType() != Material::MediumType::NONE)
        {
            return (true);
        }
    }

    return (false);
}

///////////////////////////////////////////////////////////////////////////////
Uint64 Scene::GetLightsCount(void) const
{
    return (mLights.size());
}

///////////////////////////////////////////////////////////////////////////////
void Scene::CreateMaterialTexture(GLuint* materialTexture) const
{
    glGenTextures(1, materialTexture);
    glBindTexture(GL_TEXTURE_2D, *materialTexture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA32F,
        (sizeof(Material) / sizeof(Vec4f)) * mMaterials.size(),
        1, 0, GL_RGBA, GL_FLOAT, &mMaterials[0]
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}

///////////////////////////////////////////////////////////////////////////////
void Scene::CreateTransformsTexture(GLuint* transformsTexture) const
{
    glGenTextures(1, transformsTexture);
    glBindTexture(GL_TEXTURE_2D, *transformsTexture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA32F,
        (sizeof(Mat4x4f) / sizeof(Vec4f)) * mTransforms.size(),
        1, 0, GL_RGBA, GL_FLOAT, &mTransforms[0]
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}

///////////////////////////////////////////////////////////////////////////////
void Scene::CreateVertexIndicesBuffer(
    GLuint* vertexIndicesBuffer, GLuint* vertexIndicesTexture
) const
{
    glGenBuffers(1, vertexIndicesBuffer);
    glBindBuffer(GL_TEXTURE_BUFFER, *vertexIndicesBuffer);
    glBufferData(
        GL_TEXTURE_BUFFER,
        sizeof(Index) * mVertexIndices.size(),
        &mVertexIndices[0], GL_STATIC_DRAW
    );
    glGenTextures(1, vertexIndicesTexture);
    glBindTexture(GL_TEXTURE_BUFFER, *vertexIndicesTexture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32I, *vertexIndicesBuffer);
}

///////////////////////////////////////////////////////////////////////////////
void Scene::CreateVerticesBuffer(
    GLuint* verticesBuffer, GLuint* verticesTexture
) const
{
    glGenBuffers(1, verticesBuffer);
    glBindBuffer(GL_TEXTURE_BUFFER, *verticesBuffer);
    glBufferData(
        GL_TEXTURE_BUFFER,
        sizeof(Vec2f) * mVerticesUVX.size(),
        &mVerticesUVX[0], GL_STATIC_DRAW
    );
    glGenTextures(1, verticesTexture);
    glBindTexture(GL_TEXTURE_BUFFER, *verticesTexture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, *verticesBuffer);
}

///////////////////////////////////////////////////////////////////////////////
void Scene::CreateNormalsBuffer(
    GLuint* normalsBuffer, GLuint* normalsTexture
) const
{
    glGenBuffers(1, normalsBuffer);
    glBindBuffer(GL_TEXTURE_BUFFER, *normalsBuffer);
    glBufferData(
        GL_TEXTURE_BUFFER,
        sizeof(Vec4f) * mNormalsUVY.size(),
        &mNormalsUVY[0], GL_STATIC_DRAW
    );
    glGenTextures(1, normalsTexture);
    glBindTexture(GL_TEXTURE_BUFFER, *normalsTexture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, *normalsBuffer);
}

///////////////////////////////////////////////////////////////////////////////
void Scene::CreateLightsTexture(GLuint* lightsTexture) const
{
    if (mLights.empty())
    {
        return;
    }

    glGenBuffers(1, lightsTexture);
    glBindTexture(GL_TEXTURE_2D, *lightsTexture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB32F,
        (sizeof(Light) / sizeof(Vec3f)) * mLights.size(),
        1, 0, GL_RGB, GL_FLOAT, &mLights[0]
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}

///////////////////////////////////////////////////////////////////////////////
void Scene::CreateTextureMapsArrayTexture(
    GLuint* textureMapsArrayTexture,
    const Renderer::Options& options
) const
{
    if (mTextures.empty())
    {
        return;
    }

    glGenTextures(1, textureMapsArrayTexture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, *textureMapsArrayTexture);
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8,
        options.textureArrayWidth, options.textureArrayHeight,
        mTextures.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
        &mTextureMapsArray[0]
    );
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

///////////////////////////////////////////////////////////////////////////////
void Scene::UpdateUniforms(UniquePtr<Shader>& shader) const
{
    if (mCamera)
    {
        mCamera->UpdateUniforms(shader);
    }
}

} // namespace Ray

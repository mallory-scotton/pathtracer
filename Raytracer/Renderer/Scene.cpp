///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Renderer/Scene.hpp"
#include "Renderer/Renderer.hpp"
#include "Maths/Vec4.hpp"
#include "Utils/Exception.hpp"
#include "Utils/Utils.hpp"
#include "Rays/BvhTranslator.hpp"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <imstb_image_resize.h>

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
Scene::Scene(const Path& filePath)
    : mInstanceModified(false)
    , mDirty(true)
    , mInitialized(false)
{
    if (!ParseSceneFile(filePath))
    {
        throw Exception(RAY_ERROR_SCENE_PARSE);
    }

    mSceneBvh = std::make_unique<Bvh>(10.f, 64, false);
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
    Map<Uint64, String> instanceMaterials;

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
                String filename = "";
                String material = "";

                for (const auto& [key, values] : object.properties)
                {
                    Uint64 n = values.size();

                    if (Utils::Equals(key, "file") && n == 1)
                    {
                        filename = values[0];
                    }
                    else if (Utils::Equals(key, "material") && n == 1)
                    {
                        material = values[0];
                    }
                }

                if (filename.empty())
                {
                    RAY_WARN("No file defined for Mesh");
                    break;
                }

                int meshID = -1;

                for (Uint64 i = 0; i < mMeshes.size(); i++)
                {
                    if (mMeshes[i]->GetName() == filename)
                    {
                        meshID = i;
                        break;
                    }
                }

                if (meshID == -1)
                {
                    mMeshes.push_back(std::make_shared<Mesh>(filename));
                    meshID = mMeshes.size() - 1;
                }

                if (!material.empty())
                {
                    instanceMaterials[mMeshInstances.size()] = material;
                }

                mMeshInstances.push_back(Mesh::Instance(object.properties));
                mMeshInstances.back().meshID = meshID;

                break;
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

    for (const auto& [instanceID, materialName] : instanceMaterials)
    {
        if (mMaterialMap.count(materialName) == 0)
        {
            RAY_WARN("Material \"" << materialName << "\" not found");
            continue;
        }

        mMeshInstances[instanceID].materialID = mMaterialMap.at(materialName);
    }

    RAY_INFO("Lights count: " << mLights.size());
    RAY_INFO("Materials count: " << mMaterials.size());
    RAY_INFO("Meshes count: " << mMeshes.size());
    RAY_INFO("Instances count: " << mMeshInstances.size());

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
bool Scene::HasAnEnvironmentMap(void) const
{
    return (mEnvMap.get() != nullptr);
}

///////////////////////////////////////////////////////////////////////////////
void Scene::CreateBVHBuffer(GLuint* bvhBuffer, GLuint* bvhTexture) const
{
    glGenBuffers(1, bvhBuffer);
    glBindBuffer(GL_TEXTURE_BUFFER, *bvhBuffer);
    glBufferData(
        GL_TEXTURE_BUFFER,
        sizeof(BvhTranslator::Node) * mBvhTranslator.GetNodeCount(),
        &mBvhTranslator.GetNodes()[0], GL_STATIC_DRAW
    );
    glGenTextures(1, bvhTexture);
    glBindTexture(GL_TEXTURE_BUFFER, *bvhTexture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, *bvhBuffer);
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
void Scene::InitUniforms(UniquePtr<Shader>& shader) const
{
    shader->Uniform("topBVHIndex", mBvhTranslator.GetTopLevelIndex());
}

///////////////////////////////////////////////////////////////////////////////
void Scene::UpdateUniforms(UniquePtr<Shader>& shader) const
{
    if (mCamera)
    {
        mCamera->UpdateUniforms(shader);
    }

    shader->Uniform("enableEnvMap", mOptions.enableEnvMap);
    shader->Uniform("envMapIntensity", mOptions.envMapIntensity);
    shader->Uniform("envMapRot", mOptions.envMapRot);
    shader->Uniform("maxDepth", mOptions.maxDepth);
    shader->Uniform("uniformLightCol", mOptions.uniformLightColor);
    shader->Uniform("roughnessMollificationAmt",
            mOptions.roughnessMollificationAmount);
}

///////////////////////////////////////////////////////////////////////////////
Renderer::Options Scene::GetRendererOptions(void) const
{
    return (mOptions);
}

///////////////////////////////////////////////////////////////////////////////
void Scene::CreateTLAS(void)
{
    Vector<BBox> bounds(mMeshInstances.size());

    for (Uint64 i = 0; i < mMeshInstances.size(); i++)
    {
        BBox bbox = mMeshes[mMeshInstances[i].meshID]->GetBVH()->GetBounds();
        Mat4x4f matrix = mMeshInstances[i].transform;

        Vec3f minBound = bbox.GetMin();
        Vec3f maxBound = bbox.GetMax();

        Vec3f right       = Vec3f(matrix[0][0], matrix[0][1], matrix[0][2]);
        Vec3f up          = Vec3f(matrix[1][0], matrix[1][1], matrix[1][2]);
        Vec3f forward     = Vec3f(matrix[2][0], matrix[2][1], matrix[2][2]);
        Vec3f translation = Vec3f(matrix[3][0], matrix[3][1], matrix[3][2]);

        Vec3f xa = right * minBound.x;
        Vec3f xb = right * maxBound.x;

        Vec3f ya = up * minBound.y;
        Vec3f yb = up * maxBound.y;

        Vec3f za = forward * minBound.z;
        Vec3f zb = forward * maxBound.z;

        minBound = Vec3f::Min(xa, xb) + Vec3f::Min(ya, yb) + Vec3f::Min(za, zb)
            + translation;
        maxBound = Vec3f::Max(xa, xb) + Vec3f::Max(ya, yb) + Vec3f::Max(za, zb)
            + translation;

        bounds[i] = BBox(minBound, maxBound);
    }

    mSceneBvh->Build(bounds);
    mSceneBounds = mSceneBvh->GetBounds();
}

///////////////////////////////////////////////////////////////////////////////
void Scene::CreateBLAS(void)
{
    for (Uint64 i = 0; i < mMeshes.size(); i++)
    {
        RAY_TRACE("Building BVH for " << mMeshes[i]->GetName());
        mMeshes[i]->Build();
    }
}

///////////////////////////////////////////////////////////////////////////////
void Scene::RebuildInstances(void)
{
    mSceneBvh.reset();
    mSceneBvh = std::make_unique<Bvh>(10.f, 64, false);

    CreateTLAS();
    mBvhTranslator.UpdateTLAS(mSceneBvh.get(), mMeshInstances);

    for (Uint64 i = 0; i < mMeshInstances.size(); i++)
    {
        mTransforms[i] = mMeshInstances[i].transform;
    }

    mInstanceModified = true;
    mDirty = true;
}

///////////////////////////////////////////////////////////////////////////////
void Scene::Process(void)
{
    RAY_TRACE("Processing Scene Data");
    CreateBLAS();

    RAY_TRACE("Building Scene BVH");
    CreateTLAS();

    RAY_TRACE("Flattening BVH");
    mBvhTranslator.Process(mSceneBvh.get(), mMeshes, mMeshInstances);

    int verticesCount = 0;
    RAY_TRACE("Copying Mesh Data");
    for (Uint64 i = 0; i < mMeshes.size(); i++)
    {
        Uint64 numIndices = mMeshes[i]->GetBVH()->GetIndicesCount();
        Vector<int> indices = mMeshes[i]->GetBVH()->GetIndices();

        for (Uint64 j = 0; j < numIndices; j++)
        {
            int index = indices[j];

            int v1 = (index * 3 + 0) + verticesCount;
            int v2 = (index * 3 + 1) + verticesCount;
            int v3 = (index * 3 + 2) + verticesCount;

            mVertexIndices.push_back(Index{v1, v2, v3});
        }

        mVerticesUVX.insert(
            mVerticesUVX.end(),
            mMeshes[i]->GetVertices().begin(),
            mMeshes[i]->GetVertices().end()
        );
        mNormalsUVY.insert(
            mNormalsUVY.end(),
            mMeshes[i]->GetNormals().begin(),
            mMeshes[i]->GetNormals().end()
        );

        verticesCount += static_cast<int>(mMeshes[i]->GetVertices().size());
    }

    RAY_TRACE("Copying Transforms");
    mTransforms.resize(mMeshInstances.size());
    for (Uint64 i = 0; i < mMeshInstances.size(); i++)
    {
        mTransforms[i] = mMeshInstances[i].transform;
    }

    if (!mTextures.empty())
    {
        RAY_TRACE("Copying and Resizing Textures");
    }

    int reqWidth = mOptions.textureArrayWidth;
    int reqHeight = mOptions.textureArrayHeight;
    int texBytes = reqWidth * reqHeight * 4;
    mTextureMapsArray.resize(texBytes * mTextures.size());

    for (Uint64 i = 0; i < mTextures.size(); i++)
    {
        int texWidth = mTextures[i]->GetWidth();
        int texHeight = mTextures[i]->GetHeight();

        if (texWidth != reqWidth || texHeight != reqHeight)
        {
            unsigned char* resizedTex = new unsigned char[texBytes];
            stbir_resize_uint8(
                &mTextures[i]->GetData()[0],
                texWidth, texHeight,
                0, resizedTex, reqWidth, reqHeight,
                0, 4
            );
            std::copy(
                resizedTex, resizedTex + texBytes,
                &mTextureMapsArray[i * texBytes]
            );
        }
        else
        {
            std::copy(
                mTextures[i]->GetData().begin(),
                mTextures[i]->GetData().end(),
                &mTextureMapsArray[i * texBytes]
            );
        }
    }

    if (!mCamera)
    {
        BBox bounds = mSceneBvh->GetBounds();
        Vec3f extents = bounds.Extents();
        Vec3f center = bounds.Center();

        Vec3f eye = Vec3f(
            center.x,
            center.y,
            center.z + Vec3f::Length(extents) * 2.f
        );

        mCamera = std::make_unique<Camera>(eye, center, 45.f);
    }

    mInitialized = true;
}

///////////////////////////////////////////////////////////////////////////////
bool Scene::IsInitialized(void) const
{
    return (mInitialized);
}

///////////////////////////////////////////////////////////////////////////////
bool Scene::IsDirty(void) const
{
    return (mDirty);
}

///////////////////////////////////////////////////////////////////////////////
void Scene::SetDirty(bool dirty)
{
    mDirty = dirty;
}

} // namespace Ray

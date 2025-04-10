///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Renderer/Scene.hpp"
#include "Renderer/Renderer.hpp"
#include "Maths/Vec4.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

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

} // namespace Ray

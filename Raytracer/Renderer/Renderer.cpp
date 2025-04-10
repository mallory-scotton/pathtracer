///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Renderer/Renderer.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
String Renderer::Options::GetPathTraceShaderDefinitions(
    const SharedPtr<Scene>& scene
) const
{
    String defines;

    if (enableEnvMap) // TODO: Check if the scene got env map
    {
        defines += "#define OPTION_ENVMAP\n";
    }

    if (enableRR)
    {
        defines += "#define OPTION_RR\n";
        defines += "#define OPTION_RR_DEPTH " + std::to_string(RRDepth) + "\n";
    }

    if (enableUniformLight)
    {
        defines += "#define OPTION_UNIFORM_LIGHT\n";
    }

    if (openglNormalMap)
    {
        defines += "#define OPTION_OPENGL_NORMALMAP\n";
    }

    if (hideEmitters)
    {
        defines += "#define OPTION_HIDE_EMITTERS\n";
    }

    if (enableBackground)
    {
        defines += "#define OPTION_BACKGROUND\n";
    }

    if (transparentBackground)
    {
        defines += "#define OPTION_TRANSPARENT_BACKGROUND\n";
    }

    // TODO: Loop through each materials to see if one alpha is not in OPAQUE
    // OPTION_ALPHA_TEST

    if (enableRoughnessMollification)
    {
        defines += "#define OPTION_ROUGHNESS_MOLLIFICATION\n";
    }

    // TODO: Loop through each materials to see if one as a medium type
    // OPTION_MEDIUM

    if (enableVolumeMIS)
    {
        defines += "#define OPTION_VOL_MIS\n";
    }

    return (defines);
}

///////////////////////////////////////////////////////////////////////////////
String Renderer::Options::GetTonemapShaderDefinitions(void) const
{
    String defines;

    if (enableBackground)
    {
        defines += "#define OPTION_BACKGROUND\n";
    }

    if (transparentBackground)
    {
        defines += "#define OPTION_TRANSPARENT_BACKGROUND\n";
    }

    return (defines);
}

///////////////////////////////////////////////////////////////////////////////
Renderer::Renderer(void)
{
    InitGPUDataBuffers();
    InitFBOs();
    InitShaders();
}

///////////////////////////////////////////////////////////////////////////////
Renderer::~Renderer()
{
    glDeleteTextures(1, &mBVHTexture);
    glDeleteTextures(1, &mVertexIndicesTexture);
    glDeleteTextures(1, &mVerticesTexture);
    glDeleteTextures(1, &mNormalsTexture);
    glDeleteTextures(1, &mMaterialsTexture);
    glDeleteTextures(1, &mTransformsTexture);
    glDeleteTextures(1, &mLightsTexture);
    glDeleteTextures(1, &mTextureMapsArrayTexture);
    glDeleteTextures(1, &mEnvMapTexture);
    glDeleteTextures(1, &mEnvMapCDFTexture);
    glDeleteTextures(1, &mPathTraceTexture);
    glDeleteTextures(1, &mPathTraceTextureLowRes);
    glDeleteTextures(1, &mAccumulationTexture);
    glDeleteTextures(1, &mTileOutputTexture[0]);
    glDeleteTextures(1, &mTileOutputTexture[1]);
    glDeleteTextures(1, &mDenoisedTexture);

    glDeleteBuffers(1, &mBVHBuffer);
    glDeleteBuffers(1, &mVertexIndicesBuffer);
    glDeleteBuffers(1, &mVerticesBuffer);
    glDeleteBuffers(1, &mNormalsBuffer);

    glDeleteFramebuffers(1, &mPathTraceFBO);
    glDeleteFramebuffers(1, &mPathTraceFBOLowRes);
    glDeleteFramebuffers(1, &mAccumulationFBO);
    glDeleteFramebuffers(1, &mOutputFBO);

    mPathTraceShader.reset();
    mPathTraceShaderLowRes.reset();
    mOutputShader.reset();
    mTonemapShader.reset();
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::InitGPUDataBuffers(void)
{
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    // TODO: Load BVH into the GPU buffers
    mScene->CreateVertexIndicesBuffer(
        &mVertexIndicesBuffer, &mVertexIndicesTexture
    );
    mScene->CreateVerticesBuffer(&mVerticesBuffer, &mVerticesTexture);
    mScene->CreateNormalsBuffer(&mNormalsBuffer, &mNormalsTexture);
    mScene->CreateMaterialTexture(&mMaterialsTexture);
    mScene->CreateTransformsTexture(&mTransformsTexture);
    mScene->CreateLightsTexture(&mLightsTexture);
    // TODO: Load Textures into the GPU buffers
    // TODO: Load EnvMap into the GPU buffers

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_BUFFER, mBVHTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_BUFFER, mVertexIndicesTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_BUFFER, mVerticesTexture);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_BUFFER, mNormalsTexture);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, mMaterialsTexture);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, mTransformsTexture);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, mLightsTexture);
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureMapsArrayTexture);
    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_2D, mEnvMapTexture);
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, mEnvMapCDFTexture);
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::InitFBOs(void)
{}

///////////////////////////////////////////////////////////////////////////////
void Renderer::InitShaders(void)
{
    static const String vertexShaderPath = "Raytracer/Shaders/Vertex.glsl";

    String shaderDefinitions = mOptions.GetPathTraceShaderDefinitions(mScene);

    mPathTraceShader.reset(new Shader(
        vertexShaderPath,
        "Raytracer/Shaders/Tile.glsl",
        shaderDefinitions
    ));

    mPathTraceShaderLowRes.reset(new Shader(
        vertexShaderPath,
        "Raytracer/Shaders/Preview.glsl",
        shaderDefinitions
    ));

    mOutputShader.reset(new Shader(
        vertexShaderPath,
        "Raytracer/Shaders/Output.glsl"
    ));

    mTonemapShader.reset(new Shader(
        vertexShaderPath,
        "Raytracer/Shaders/Tonemap.glsl",
        mOptions.GetTonemapShaderDefinitions()
    ));

    // TODO: Setup the uniforms
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::Render(void)
{}

///////////////////////////////////////////////////////////////////////////////
void Renderer::Update(float deltaSeconds)
{
    RAY_UNUSED(deltaSeconds);
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::ReloadShaders(void)
{
    InitShaders();
}

} // namespace Ray

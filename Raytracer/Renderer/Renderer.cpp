///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Renderer/Renderer.hpp"
#include "Renderer/Scene.hpp"
#include <cmath>

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

    if (scene->IsUsingLights())
    {
        defines += "#define OPTION_LIGHTS\n";
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

    if (scene->IsUsingAlphaTest())
    {
        defines += "#define OPTION_ALPHA_TEST\n";
    }

    if (enableRoughnessMollification)
    {
        defines += "#define OPTION_ROUGHNESS_MOLLIFICATION\n";
    }

    if (scene->IsUsingMedium())
    {
        defines += "#define OPTION_MEDIUM\n";
    }

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
Renderer::Renderer(SharedPtr<Scene> scene)
    : mScene(scene)
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
    mScene->CreateTextureMapsArrayTexture(&mTextureMapsArrayTexture, mOptions);
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
void Renderer::InitPathTraceFBO(void)
{
    glGenFramebuffers(1, &mPathTraceFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mPathTraceFBO);

    glGenTextures(1, &mPathTraceTexture);
    glBindTexture(GL_TEXTURE_2D, mPathTraceTexture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA32F,
        mTileWidth, mTileHeight, 0,
        GL_RGBA, GL_FLOAT, 0
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, mPathTraceTexture, 0
    );
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::InitPathTraceLowResFBO(void)
{
    glGenFramebuffers(1, &mPathTraceFBOLowRes);
    glBindFramebuffer(GL_FRAMEBUFFER, mPathTraceFBOLowRes);

    glGenTextures(1, &mPathTraceTextureLowRes);
    glBindTexture(GL_TEXTURE_2D, mPathTraceTextureLowRes);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA32F,
        mWindowSize.x * mPixelRatio, mWindowSize.y * mPixelRatio, 0,
        GL_RGBA, GL_FLOAT, 0
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, mPathTraceTextureLowRes, 0
    );
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::InitAccumulationFBO(void)
{
    glGenFramebuffers(1, &mAccumulationFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mAccumulationFBO);

    glGenTextures(1, &mAccumulationTexture);
    glBindTexture(GL_TEXTURE_2D, mAccumulationTexture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA32F,
        mRenderSize.x, mRenderSize.y, 0,
        GL_RGBA, GL_FLOAT, 0
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, mAccumulationTexture, 0
    );
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::InitOutputFBO(void)
{
    glGenFramebuffers(1, &mOutputFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mOutputFBO);

    glGenTextures(1, &mTileOutputTexture[0]);
    glBindTexture(GL_TEXTURE_2D, mTileOutputTexture[0]);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA32F,
        mRenderSize.x, mRenderSize.y, 0,
        GL_RGBA, GL_FLOAT, 0
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &mTileOutputTexture[1]);
    glBindTexture(GL_TEXTURE_2D, mTileOutputTexture[1]);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA32F,
        mRenderSize.x, mRenderSize.y, 0,
        GL_RGBA, GL_FLOAT, 0
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, mTileOutputTexture[mCurrentBuffer], 0
    );
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::InitFBOs(void)
{
    mSampleCounter = 1;
    mCurrentBuffer = 0;
    mFrameCounter = 1;

    mRenderSize = mOptions.renderResolution;
    mWindowSize = mOptions.windowResolution;

    mTileWidth = mOptions.tileWidth;
    mTileHeight = mOptions.tileHeight;

    mInvTileCount = static_cast<float>(mTileWidth / mRenderSize.x);
    mInvTileCount = static_cast<float>(mTileHeight / mRenderSize.y);

    mTileCount.x = std::ceil(static_cast<float>(mRenderSize.x / mTileWidth));
    mTileCount.y = std::ceil(static_cast<float>(mRenderSize.y / mTileHeight));

    mTile.x = -1;
    mTile.y = mTileCount.y - 1;

    InitPathTraceFBO();
    InitPathTraceLowResFBO();
    InitAccumulationFBO();
    InitOutputFBO();

    // TODO: Add denoiser

    RAY_INFO("Window Resolution: " << mWindowSize);
    RAY_INFO("Render Resolution: " << mRenderSize);
    RAY_INFO("Preview Resolution: " << Vec2i(
        mWindowSize.x * mPixelRatio, mWindowSize.y * mPixelRatio
    ));
    RAY_INFO("Tile Size: " << Vec2i(mTileWidth, mTileHeight));
}

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
{
    if (mDirty)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mPathTraceFBOLowRes);
        glViewport(
            0, 0, mWindowSize.x * mPixelRatio, mWindowSize.y * mPixelRatio
        );

        mPathTraceShaderLowRes->Use();
        mQuad.Draw();
        mPathTraceShaderLowRes->StopUsing();

        mDirty = false;
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mPathTraceFBO);
        glViewport(0, 0, mTileWidth, mTileHeight);
        glBindTexture(GL_TEXTURE_2D, mAccumulationTexture);

        mPathTraceShader->Use();
        mQuad.Draw();
        mPathTraceShader->StopUsing();

        glBindFramebuffer(GL_FRAMEBUFFER, mAccumulationFBO);
        glViewport(
            mTileWidth * mTile.x,
            mTileHeight * mTile.y,
            mTileWidth, mTileHeight
        );
        glBindTexture(GL_TEXTURE_2D, mPathTraceTexture);

        mOutputShader->Use();
        mQuad.Draw();
        mOutputShader->StopUsing();

        glBindFramebuffer(GL_FRAMEBUFFER, mOutputFBO);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, mTileOutputTexture[mCurrentBuffer], 0
        );
        glViewport(0, 0, mRenderSize.x, mRenderSize.y);
        glBindTexture(GL_TEXTURE_2D, mAccumulationTexture);

        mTonemapShader->Use();
        mQuad.Draw();
        mTonemapShader->StopUsing();
    }
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::DrawToScreen(void)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, mWindowSize.x, mWindowSize.y);
    glBindTexture(GL_TEXTURE_2D, mTileOutputTexture[mCurrentBuffer]);

    mTonemapShader->Use();
    mQuad.Draw();
    mTonemapShader->StopUsing();
}

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

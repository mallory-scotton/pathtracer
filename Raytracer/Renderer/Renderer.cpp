///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Renderer/Renderer.hpp"
#include "Renderer/Scene.hpp"
#include "Utils/Utils.hpp"
#include <cmath>

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
Renderer::Options::Options(const Map<String, Vector<String>>& props)
{
    for (const auto& [key, values] : props)
    {
        Uint64 n = values.size();

        if (Utils::Equals(key, "envMap") && n == 1)
        {
            if (Utils::Equals(values[0], "none"))
            {
                enableEnvMap = false;
            }
            else
            {
                // TODO: Add the envMap
                enableEnvMap = true;
            }
        }
        else if (Utils::Equals(key, "resolution") && n == 2)
        {
            renderResolution = Vec2i(
                Utils::ToInteger(values[0]),
                Utils::ToInteger(values[1])
            );
        }
        else if (Utils::Equals(key, "windowResolution") && n == 2)
        {
            windowResolution = Vec2i(
                Utils::ToInteger(values[0]),
                Utils::ToInteger(values[1])
            );
        }
        else if (Utils::Equals(key, "envMapIntensity") && n == 1)
        {
            envMapIntensity = Utils::ToFloat(values[0]);
        }
        else if (Utils::Equals(key, "maxDepth") && n == 1)
        {
            maxDepth = Utils::ToInteger(values[0]);
        }
        else if (Utils::Equals(key, "maxSSP") && n == 1)
        {
            maxSPP = Utils::ToInteger(values[0]);
        }
        else if (Utils::Equals(key, "tileWidth") && n == 1)
        {
            tileWidth = Utils::ToInteger(values[0]);
        }
        else if (Utils::Equals(key, "tileHeight") && n == 1)
        {
            tileHeight = Utils::ToInteger(values[0]);
        }
        else if (Utils::Equals(key, "enableRR") && n == 1)
        {
            enableRR = Utils::ToBoolean(values[0]);
        }
        else if (Utils::Equals(key, "RRDepth") && n == 1)
        {
            RRDepth = Utils::ToInteger(values[0]);
        }
        else if (Utils::Equals(key, "enableTonemap") && n == 1)
        {
            enableTonemap = Utils::ToBoolean(values[0]);
        }
        else if (Utils::Equals(key, "EnableAces") && n == 1)
        {
            enableAces = Utils::ToBoolean(values[0]);
        }
        else if (Utils::Equals(key, "textureArrayWidth") && n == 1)
        {
            textureArrayWidth = Utils::ToInteger(values[0]);
        }
        else if (Utils::Equals(key, "textureArrayHeight") && n == 1)
        {
            textureArrayHeight = Utils::ToInteger(values[0]);
        }
        else if (Utils::Equals(key, "openglNormalMap") && n == 1)
        {
            openglNormalMap = Utils::ToBoolean(values[0]);
        }
        else if (Utils::Equals(key, "hideEmitters") && n == 1)
        {
            hideEmitters = Utils::ToBoolean(values[0]);
        }
        else if (Utils::Equals(key, "enableBackground") && n == 1)
        {
            enableBackground = Utils::ToBoolean(values[0]);
        }
        else if (Utils::Equals(key, "transparentBackground") && n == 1)
        {
            transparentBackground = Utils::ToBoolean(values[0]);
        }
        else if (Utils::Equals(key, "backgroundColor") && n == 3)
        {
            backgroundColor = Color(
                Utils::ToFloat(values[0]),
                Utils::ToFloat(values[1]),
                Utils::ToFloat(values[2])
            );
        }
        else if (Utils::Equals(key, "independentRenderSize") && n == 1)
        {
            independantRenderSize = Utils::ToBoolean(values[0]);
        }
        else if (Utils::Equals(key, "envMapRotation") && n == 1)
        {
            envMapRot = Utils::ToFloat(values[0]);
        }
        else if (Utils::Equals(key, "enableRoughnessMollification") && n == 1)
        {
            enableRoughnessMollification = Utils::ToBoolean(values[0]);
        }
        else if (Utils::Equals(key, "roughnessMollificationAmount") && n == 1)
        {
            roughnessMollificationAmount = Utils::ToFloat(values[0]);
        }
        else if (Utils::Equals(key, "enableVolumeMIS") && n == 1)
        {
            enableVolumeMIS = Utils::ToBoolean(values[0]);
        }
        else if (Utils::Equals(key, "enableUniformLight") && n == 1)
        {
            enableUniformLight = Utils::ToBoolean(values[0]);
        }
        else if (Utils::Equals(key, "uniformLightColor") && n == 3)
        {
            uniformLightColor = Color(
                Utils::ToFloat(values[0]),
                Utils::ToFloat(values[1]),
                Utils::ToFloat(values[2])
            );
        }
        else
        {
            RAY_WARN(
                "\"" + key + "\" is not a valid Renderer Options attributes"
            );
        }
    }

    if (!independantRenderSize)
    {
        windowResolution = renderResolution;
    }
}

///////////////////////////////////////////////////////////////////////////////
String Renderer::Options::GetPathTraceShaderDefinitions(
    const SharedPtr<Scene>& scene
) const
{
    String defines;

    if (enableEnvMap && scene->HasAnEnvironmentMap())
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
    , mOptions(scene->GetRendererOptions())
{
    if (!mScene->IsInitialized())
    {
        mScene->Process();
    }

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

    mScene->CreateBVHBuffer(&mBVHBuffer, &mBVHTexture);
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

    InitUniforms(mPathTraceShader);
    InitUniforms(mPathTraceShaderLowRes);
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::InitUniforms(UniquePtr<Shader>& shader)
{
    shader->Use();

    if  (false) // TODO: Scene Env Map
    {
        // TODO: envMapRes
        // TODO: envMapTotalSum
    }

    mScene->InitUniforms(shader);
    shader->Uniform("resolution", Vec2f(mRenderSize));
    shader->Uniform("invNumTiles", mInvTileCount);
    shader->Uniform("numOfLights", static_cast<int>(mScene->GetLightsCount()));
    shader->Uniform("accumTexture", 0);
    shader->Uniform("BVH", 1);
    shader->Uniform("vertexIndicesTex", 2);
    shader->Uniform("verticesTex", 3);
    shader->Uniform("normalsTex", 4);
    shader->Uniform("materialsTex", 5);
    shader->Uniform("transformsTex", 6);
    shader->Uniform("lightsTex", 7);
    shader->Uniform("textureMapsArrayTex", 8);
    shader->Uniform("envMapTex", 9);
    shader->Uniform("envMapCDFTex", 10);

    shader->StopUsing();
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::Render(void)
{
    if (
        !mScene->IsDirty() &&
        mOptions.maxSPP != -1 &&
        mSampleCounter >= mOptions.maxSPP
    )
    {
        return;
    }

    glActiveTexture(GL_TEXTURE0);

    if (mScene->IsDirty())
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mPathTraceFBOLowRes);
        glViewport(
            0, 0, mWindowSize.x * mPixelRatio, mWindowSize.y * mPixelRatio
        );

        mPathTraceShaderLowRes->Use();
        mQuad.Draw();
        mPathTraceShaderLowRes->StopUsing();

        mScene->SetDirty(false);
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
void Renderer::Present(void)
{
    glActiveTexture(GL_TEXTURE0);

    if (mScene->IsDirty() || mSampleCounter == 1)
    {
        glBindTexture(GL_TEXTURE_2D, mPathTraceTextureLowRes);
        mTonemapShader->Use();
        mQuad.Draw();
        mTonemapShader->StopUsing();
    }
    else
    {
        if (false) // TODO: Bind denoised texture
        {

        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, mTileOutputTexture[1 - mCurrentBuffer]);
        }

        mOutputShader->Use();
        mQuad.Draw();
        mOutputShader->StopUsing();
    }
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::DrawToScreen(void)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, mOptions.windowResolution.x, mOptions.windowResolution.y);
    Present();
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::Update(float deltaSeconds)
{
    RAY_UNUSED(deltaSeconds);

    if (
        !mScene->IsDirty() &&
        mOptions.maxSPP != -1 &&
        mSampleCounter >= mOptions.maxSPP
    )
    {
        return;
    }

    if (false) // TODO: Update if instance has been modified
    {

    }

    if (false) // TODO: Update when env map has been modified
    {

    }

    if (false) // TODO: Update when denoiser request is ask
    {

    }

    if (mScene->IsDirty())
    {
        mTile.x = -1.f;
        mTile.y = mTileCount.y - 1.f;
        mSampleCounter = 1;
        mFrameCounter = 1;

        glBindFramebuffer(GL_FRAMEBUFFER, mAccumulationFBO);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    else
    {
        mFrameCounter++;
        mTile.x++;
        if (mTile.x >= mTileCount.x)
        {
            mTile.x = 0.f;
            mTile.y--;
            if (mTile.y < 0.f)
            {
                mTile.x = 0.f;
                mTile.y = mTileCount.y - 1.f;
                mSampleCounter++;
                mCurrentBuffer = 1 - mCurrentBuffer;
            }
        }
    }

    mPathTraceShader->Use();
    mScene->UpdateUniforms(mPathTraceShader);
    mPathTraceShader->Uniform("tileOffset", Vec2f(
        static_cast<float>(mTile.x) * mInvTileCount.x,
        static_cast<float>(mTile.y) * mInvTileCount.y
    ));
    mPathTraceShader->Uniform("frameNum", mFrameCounter);
    mPathTraceShader->StopUsing();

    mPathTraceShaderLowRes->Use();
    mScene->UpdateUniforms(mPathTraceShaderLowRes);
    mPathTraceShaderLowRes->Uniform("tileOffset", Vec2f(
        static_cast<float>(mTile.x) * mInvTileCount.x,
        static_cast<float>(mTile.y) * mInvTileCount.y
    ));
    mPathTraceShaderLowRes->Uniform("frameNum", mFrameCounter);
    mPathTraceShaderLowRes->StopUsing();

    mTonemapShader->Use();
    mTonemapShader->Uniform("invSampleCounter", 1.f / mSampleCounter);
    mTonemapShader->Uniform("enableTonemap", mOptions.enableTonemap);
    mTonemapShader->Uniform("enableAces", mOptions.enableAces);
    mTonemapShader->Uniform("simpleAcesFit", mOptions.simpleAcesFit);
    mTonemapShader->Uniform("backgroundCol", mOptions.backgroundColor);
    mTonemapShader->StopUsing();
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::ReloadShaders(void)
{
    InitShaders();
}

} // namespace Ray

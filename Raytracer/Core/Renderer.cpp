///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Config.hpp"
#include "Core/Renderer.hpp"
#include "Components/Scene.h"
#include "OpenImageDenoise/oidn.hpp"
#include "Utils/OpenGL.hpp"
#include "Core/Context.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
Renderer::Options::Options(void)
    : renderResolution(1280, 720)
    , windowResolution(1280, 720)
    , uniformLightCol(0.3f)
    , backgroundCol(1.0f)
    , tileWidth(100)
    , tileHeight(100)
    , maxDepth(2)
    , maxSpp(-1)
    , RRDepth(2)
    , texArrayWidth(2048)
    , texArrayHeight(2048)
    , denoiserFrameCnt(20)
    , enableRR(true)
    , enableDenoiser(false)
    , enableTonemap(true)
    , enableAces(false)
    , simpleAcesFit(false)
    , openglNormalMap(true)
    , enableEnvMap(false)
    , enableUniformLight(false)
    , hideEmitters(false)
    , enableBackground(false)
    , transparentBackground(false)
    , independentRenderSize(false)
    , enableRoughnessMollification(false)
    , enableVolumeMIS(false)
    , envMapIntensity(1.0f)
    , envMapRot(0.0f)
    , roughnessMollificationAmt(0.0f)
{}

///////////////////////////////////////////////////////////////////////////////
Renderer::Renderer(void)
    : BVHBuffer(0)
    , BVHTex(0)
    , vertexIndicesBuffer(0)
    , vertexIndicesTex(0)
    , verticesBuffer(0)
    , verticesTex(0)
    , normalsBuffer(0)
    , normalsTex(0)
    , materialsTex(0)
    , transformsTex(0)
    , lightsTex(0)
    , textureMapsArrayTex(0)
    , envMapTex(0)
    , envMapCDFTex(0)
    , pathTraceFBO(0)
    , pathTraceFBOLowRes(0)
    , accumFBO(0)
    , outputFBO(0)
    , pathTraceTextureLowRes(0)
    , pathTraceTexture(0)
    , accumTexture(0)
    , denoisedTexture(0)
    , currentBuffer(0)
    , frameCounter(0)
    , sampleCounter(0)
    , pixelRatio(1.0f)
    , denoised(false)
{
    Context& ctx = Context::GetInstance();

    if (!ctx.scene->initialized)
    {
        ctx.scene->ProcessScene();
    }

    InitGPUDataBuffers();
    InitFBOs();
    InitShaders();
}

    Renderer::~Renderer()
    {
        // Delete textures
        glDeleteTextures(1, &BVHTex);
        glDeleteTextures(1, &vertexIndicesTex);
        glDeleteTextures(1, &verticesTex);
        glDeleteTextures(1, &normalsTex);
        glDeleteTextures(1, &materialsTex);
        glDeleteTextures(1, &transformsTex);
        glDeleteTextures(1, &lightsTex);
        glDeleteTextures(1, &textureMapsArrayTex);
        glDeleteTextures(1, &envMapTex);
        glDeleteTextures(1, &envMapCDFTex);
        glDeleteTextures(1, &pathTraceTexture);
        glDeleteTextures(1, &pathTraceTextureLowRes);
        glDeleteTextures(1, &accumTexture);
        glDeleteTextures(1, &tileOutputTexture[0]);
        glDeleteTextures(1, &tileOutputTexture[1]);
        glDeleteTextures(1, &denoisedTexture);

        // Delete buffers
        glDeleteBuffers(1, &BVHBuffer);
        glDeleteBuffers(1, &vertexIndicesBuffer);
        glDeleteBuffers(1, &verticesBuffer);
        glDeleteBuffers(1, &normalsBuffer);

        // Delete FBOs
        glDeleteFramebuffers(1, &pathTraceFBO);
        glDeleteFramebuffers(1, &pathTraceFBOLowRes);
        glDeleteFramebuffers(1, &accumFBO);
        glDeleteFramebuffers(1, &outputFBO);

        // Delete shaders
        pathTraceShader.reset();
        pathTraceShaderLowRes.reset();
        outputShader.reset();
        tonemapShader.reset();

        // Delete denoiser data
        delete[] denoiserInputFramePtr;
        delete[] frameOutputPtr;

    }

    void Renderer::InitGPUDataBuffers()
    {
        Context& ctx = Context::GetInstance();

        glPixelStorei(GL_PACK_ALIGNMENT, 1);

        // Create buffer and texture for BVH
        glGenBuffers(1, &BVHBuffer);
        glBindBuffer(GL_TEXTURE_BUFFER, BVHBuffer);
        glBufferData(GL_TEXTURE_BUFFER, sizeof(Ray::BvhTranslator::Node) * ctx.scene->bvhTranslator.nodes.size(), &ctx.scene->bvhTranslator.nodes[0], GL_STATIC_DRAW);
        glGenTextures(1, &BVHTex);
        glBindTexture(GL_TEXTURE_BUFFER, BVHTex);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, BVHBuffer);

        // Create buffer and texture for vertex indices
        glGenBuffers(1, &vertexIndicesBuffer);
        glBindBuffer(GL_TEXTURE_BUFFER, vertexIndicesBuffer);
        glBufferData(GL_TEXTURE_BUFFER, sizeof(Indices) * ctx.scene->vertIndices.size(), &ctx.scene->vertIndices[0], GL_STATIC_DRAW);
        glGenTextures(1, &vertexIndicesTex);
        glBindTexture(GL_TEXTURE_BUFFER, vertexIndicesTex);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32I, vertexIndicesBuffer);

        // Create buffer and texture for vertices
        glGenBuffers(1, &verticesBuffer);
        glBindBuffer(GL_TEXTURE_BUFFER, verticesBuffer);
        glBufferData(GL_TEXTURE_BUFFER, sizeof(Vec4f) * ctx.scene->verticesUVX.size(), &ctx.scene->verticesUVX[0], GL_STATIC_DRAW);
        glGenTextures(1, &verticesTex);
        glBindTexture(GL_TEXTURE_BUFFER, verticesTex);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, verticesBuffer);

        // Create buffer and texture for normals
        glGenBuffers(1, &normalsBuffer);
        glBindBuffer(GL_TEXTURE_BUFFER, normalsBuffer);
        glBufferData(GL_TEXTURE_BUFFER, sizeof(Vec4f) * ctx.scene->normalsUVY.size(), &ctx.scene->normalsUVY[0], GL_STATIC_DRAW);
        glGenTextures(1, &normalsTex);
        glBindTexture(GL_TEXTURE_BUFFER, normalsTex);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, normalsBuffer);

        // Create texture for materials
        glGenTextures(1, &materialsTex);
        glBindTexture(GL_TEXTURE_2D, materialsTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, (sizeof(Material) / sizeof(Vec4f)) * ctx.scene->materials.size(), 1, 0, GL_RGBA, GL_FLOAT, &ctx.scene->materials[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Create texture for transforms
        glGenTextures(1, &transformsTex);
        glBindTexture(GL_TEXTURE_2D, transformsTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, (sizeof(Mat4x4f) / sizeof(Vec4f)) * ctx.scene->transforms.size(), 1, 0, GL_RGBA, GL_FLOAT, &ctx.scene->transforms[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Create texture for lights
        if (!ctx.scene->lights.empty())
        {
            //Create texture for lights
            glGenTextures(1, &lightsTex);
            glBindTexture(GL_TEXTURE_2D, lightsTex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, (sizeof(Light) / sizeof(Vec3f)) * ctx.scene->lights.size(), 1, 0, GL_RGB, GL_FLOAT, &ctx.scene->lights[0]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // Create texture for scene textures
        if (!ctx.scene->textures.empty())
        {
            glGenTextures(1, &textureMapsArrayTex);
            glBindTexture(GL_TEXTURE_2D_ARRAY, textureMapsArrayTex);
            glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, ctx.scene->renderOptions.texArrayWidth, ctx.scene->renderOptions.texArrayHeight, ctx.scene->textures.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE, &ctx.scene->textureMapsArray[0]);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        }

        // Create texture for environment map
        if (ctx.scene->envMap != nullptr)
        {
            glGenTextures(1, &envMapTex);
            glBindTexture(GL_TEXTURE_2D, envMapTex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, ctx.scene->envMap->width, ctx.scene->envMap->height, 0, GL_RGB, GL_FLOAT, ctx.scene->envMap->img.get());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);

            glGenTextures(1, &envMapCDFTex);
            glBindTexture(GL_TEXTURE_2D, envMapCDFTex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, ctx.scene->envMap->width, ctx.scene->envMap->height, 0, GL_RED, GL_FLOAT, ctx.scene->envMap->cdf.get());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // Bind textures to texture slots as they will not change slots during the lifespan of the renderer
        OpenGL::Texture::Active(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_BUFFER, BVHTex);
        OpenGL::Texture::Active(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_BUFFER, vertexIndicesTex);
        OpenGL::Texture::Active(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_BUFFER, verticesTex);
        OpenGL::Texture::Active(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_BUFFER, normalsTex);
        OpenGL::Texture::Active(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, materialsTex);
        OpenGL::Texture::Active(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, transformsTex);
        OpenGL::Texture::Active(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, lightsTex);
        OpenGL::Texture::Active(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureMapsArrayTex);
        OpenGL::Texture::Active(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, envMapTex);
        OpenGL::Texture::Active(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, envMapCDFTex);
    }

    void Renderer::ResizeRenderer()
    {
        // Delete textures
        glDeleteTextures(1, &pathTraceTexture);
        glDeleteTextures(1, &pathTraceTextureLowRes);
        glDeleteTextures(1, &accumTexture);
        glDeleteTextures(1, &tileOutputTexture[0]);
        glDeleteTextures(1, &tileOutputTexture[1]);
        glDeleteTextures(1, &denoisedTexture);

        // Delete FBOs
        glDeleteFramebuffers(1, &pathTraceFBO);
        glDeleteFramebuffers(1, &pathTraceFBOLowRes);
        glDeleteFramebuffers(1, &accumFBO);
        glDeleteFramebuffers(1, &outputFBO);

        // Delete denoiser data
        delete[] denoiserInputFramePtr;
        delete[] frameOutputPtr;

        // Delete shaders
        pathTraceShader.reset();
        pathTraceShaderLowRes.reset();
        outputShader.reset();
        tonemapShader.reset();

        InitFBOs();
        InitShaders();
    }

    void Renderer::InitFBOs()
    {
        Context& ctx = Context::GetInstance();

        sampleCounter = 1;
        currentBuffer = 0;
        frameCounter = 1;

        renderSize = ctx.scene->renderOptions.renderResolution;
        windowSize = ctx.scene->renderOptions.windowResolution;

        tileWidth = ctx.scene->renderOptions.tileWidth;
        tileHeight = ctx.scene->renderOptions.tileHeight;

        invNumTiles.x = (float)tileWidth / renderSize.x;
        invNumTiles.y = (float)tileHeight / renderSize.y;

        numTiles.x = ceil((float)renderSize.x / tileWidth);
        numTiles.y = ceil((float)renderSize.y / tileHeight);

        tile.x = -1;
        tile.y = numTiles.y - 1;

        // Create FBOs for path trace shader
        glGenFramebuffers(1, &pathTraceFBO);
        OpenGL::BindFramebuffer(pathTraceFBO);

        // Create Texture for FBO
        glGenTextures(1, &pathTraceTexture);
        glBindTexture(GL_TEXTURE_2D, pathTraceTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tileWidth, tileHeight, 0, GL_RGBA, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pathTraceTexture, 0);

        // Create FBOs for low res preview shader
        glGenFramebuffers(1, &pathTraceFBOLowRes);
        OpenGL::BindFramebuffer(pathTraceFBOLowRes);

        // Create Texture for FBO
        glGenTextures(1, &pathTraceTextureLowRes);
        glBindTexture(GL_TEXTURE_2D, pathTraceTextureLowRes);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, windowSize.x * pixelRatio, windowSize.y * pixelRatio, 0, GL_RGBA, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pathTraceTextureLowRes, 0);

        // Create FBOs for accum buffer
        glGenFramebuffers(1, &accumFBO);
        OpenGL::BindFramebuffer(accumFBO);

        // Create Texture for FBO
        glGenTextures(1, &accumTexture);
        glBindTexture(GL_TEXTURE_2D, accumTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, renderSize.x, renderSize.y, 0, GL_RGBA, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumTexture, 0);

        // Create FBOs for tile output shader
        glGenFramebuffers(1, &outputFBO);
        OpenGL::BindFramebuffer(outputFBO);

        // Create Texture for FBO
        glGenTextures(1, &tileOutputTexture[0]);
        glBindTexture(GL_TEXTURE_2D, tileOutputTexture[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, renderSize.x, renderSize.y, 0, GL_RGBA, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenTextures(1, &tileOutputTexture[1]);
        glBindTexture(GL_TEXTURE_2D, tileOutputTexture[1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, renderSize.x, renderSize.y, 0, GL_RGBA, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tileOutputTexture[currentBuffer], 0);

        // For Denoiser
        denoiserInputFramePtr = new Vec3f[renderSize.x * renderSize.y];
        frameOutputPtr = new Vec3f[renderSize.x * renderSize.y];

        glGenTextures(1, &denoisedTexture);
        glBindTexture(GL_TEXTURE_2D, denoisedTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, renderSize.x, renderSize.y, 0, GL_RGB, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        printf("Window Resolution : %d %d\n", windowSize.x, windowSize.y);
        printf("Render Resolution : %d %d\n", renderSize.x, renderSize.y);
        printf("Preview Resolution : %d %d\n", (int)((float)windowSize.x * pixelRatio), (int)((float)windowSize.y * pixelRatio));
        printf("Tile Size : %d %d\n", tileWidth, tileHeight);
    }

    void Renderer::ReloadShaders()
    {
        // Delete shaders
        pathTraceShader.reset();
        pathTraceShaderLowRes.reset();
        outputShader.reset();
        tonemapShader.reset();

        InitShaders();
    }

    void Renderer::InitShaders()
    {
        Context& ctx = Context::GetInstance();
        String shadersDirectory = ctx.GetShaderPath();

        String vertexShaderSrc = Shader::Source(shadersDirectory + "Vertex/Vertex.glsl");
        String pathTraceShaderSrc = Shader::Source(shadersDirectory + "Lighting/Tile.glsl");
        String pathTraceShaderLowResSrc = Shader::Source(shadersDirectory + "Stages/Preview.glsl");
        String outputShaderSrc = Shader::Source(shadersDirectory + "Stages/Output.glsl");
        String tonemapShaderSrc = Shader::Source(shadersDirectory + "Lighting/Tonemap.glsl");

        // Add preprocessor defines for conditional compilation
        std::string pathtraceDefines = "";
        std::string tonemapDefines = "";

        if (ctx.scene->renderOptions.enableEnvMap && ctx.scene->envMap != nullptr)
            pathtraceDefines += "#define OPT_ENVMAP\n";

        if (!ctx.scene->lights.empty())
            pathtraceDefines += "#define OPT_LIGHTS\n";

        if (ctx.scene->renderOptions.enableRR)
        {
            pathtraceDefines += "#define OPT_RR\n";
            pathtraceDefines += "#define OPT_RR_DEPTH " + std::to_string(ctx.scene->renderOptions.RRDepth) + "\n";
        }

        if (ctx.scene->renderOptions.enableUniformLight)
            pathtraceDefines += "#define OPT_UNIFORM_LIGHT\n";

        if (ctx.scene->renderOptions.openglNormalMap)
            pathtraceDefines += "#define OPT_OPENGL_NORMALMAP\n";

        if (ctx.scene->renderOptions.hideEmitters)
            pathtraceDefines += "#define OPT_HIDE_EMITTERS\n";

        if (ctx.scene->renderOptions.enableBackground)
        {
            pathtraceDefines += "#define OPT_BACKGROUND\n";
            tonemapDefines += "#define OPT_BACKGROUND\n";
        }

        if (ctx.scene->renderOptions.transparentBackground)
        {
            pathtraceDefines += "#define OPT_TRANSPARENT_BACKGROUND\n";
            tonemapDefines += "#define OPT_TRANSPARENT_BACKGROUND\n";
        }

        for (Uint64 i = 0; i < ctx.scene->materials.size(); i++)
        {
            if ((int)ctx.scene->materials[i].alphaMode != Material::OPAQUE)
            {
                pathtraceDefines += "#define OPT_ALPHA_TEST\n";
                break;
            }
        }

        if (ctx.scene->renderOptions.enableRoughnessMollification)
            pathtraceDefines += "#define OPT_ROUGHNESS_MOLLIFICATION\n";

        for (Uint64 i = 0; i < ctx.scene->materials.size(); i++)
        {
            if ((int)ctx.scene->materials[i].mediumType != Material::NONE)
            {
                pathtraceDefines += "#define OPT_MEDIUM\n";
                break;
            }
        }

        if (ctx.scene->renderOptions.enableVolumeMIS)
            pathtraceDefines += "#define OPT_VOL_MIS\n";

        if (pathtraceDefines.size() > 0)
        {
            size_t idx = pathTraceShaderSrc.find("#version");
            if (idx != String::npos)
                idx = pathTraceShaderSrc.find("\n", idx);
            else
                idx = 0;
            pathTraceShaderSrc.insert(idx + 1, pathtraceDefines);

            idx = pathTraceShaderLowResSrc.find("#version");
            if (idx != String::npos)
                idx = pathTraceShaderLowResSrc.find("\n", idx);
            else
                idx = 0;
            pathTraceShaderLowResSrc.insert(idx + 1, pathtraceDefines);
        }

        if (tonemapDefines.size() > 0)
        {
            size_t idx = tonemapShaderSrc.find("#version");
            if (idx != String::npos)
                idx = tonemapShaderSrc.find("\n", idx);
            else
                idx = 0;
            tonemapShaderSrc.insert(idx + 1, tonemapDefines);
        }

        pathTraceShader = std::make_unique<Shader>(vertexShaderSrc, pathTraceShaderSrc);
        pathTraceShaderLowRes = std::make_unique<Shader>(vertexShaderSrc, pathTraceShaderLowResSrc);
        outputShader = std::make_unique<Shader>(vertexShaderSrc, outputShaderSrc);
        tonemapShader = std::make_unique<Shader>(vertexShaderSrc, tonemapShaderSrc);

        InitializeUniforms(pathTraceShader);
        InitializeUniforms(pathTraceShaderLowRes);
    }

    void Renderer::InitializeUniforms(UniquePtr<Shader>& shader)
    {
        Context& ctx = Context::GetInstance();

        shader->Use();

        if (ctx.scene->envMap)
        {
            shader->Uniform("envMapRes",Vec2f(
                ctx.scene->envMap->width,
                ctx.scene->envMap->height
            ));
            shader->Uniform("envMapTotalSum", ctx.scene->envMap->totalSum);
        }

        shader->Uniform("topBVHIndex", ctx.scene->bvhTranslator.topLevelIndex);
        shader->Uniform("resolution", Vec2f(renderSize));
        shader->Uniform("invNumTiles", invNumTiles);
        shader->Uniform("numOfLights", static_cast<int>(ctx.scene->lights.size()));
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

    void Renderer::Render()
    {
        Context& ctx = Context::GetInstance();

        if (!ctx.scene->dirty && ctx.scene->renderOptions.maxSpp != -1 && sampleCounter >= ctx.scene->renderOptions.maxSpp)
            return;

        OpenGL::Disable(GL_BLEND);
        OpenGL::Disable(GL_DEPTH_TEST);
        OpenGL::Texture::Active(GL_TEXTURE0);

        if (ctx.scene->dirty)
        {
            // Renders a low res preview if camera/instances are modified
            OpenGL::BindFramebuffer(pathTraceFBOLowRes);
            glViewport(0, 0, windowSize.x * pixelRatio, windowSize.y * pixelRatio);
            quad.Draw(pathTraceShaderLowRes);

            ctx.scene->instancesModified = false;
            ctx.scene->dirty = false;
            ctx.scene->envMapModified = false;
        }
        else
        {
            OpenGL::BindFramebuffer(pathTraceFBO);
            glViewport(0, 0, tileWidth, tileHeight);
            glBindTexture(GL_TEXTURE_2D, accumTexture);
            quad.Draw(pathTraceShader);

            OpenGL::BindFramebuffer(accumFBO);
            glViewport(tileWidth * tile.x, tileHeight * tile.y, tileWidth, tileHeight);
            glBindTexture(GL_TEXTURE_2D, pathTraceTexture);
            quad.Draw(outputShader);

            OpenGL::BindFramebuffer(outputFBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tileOutputTexture[currentBuffer], 0);
            glViewport(0, 0, renderSize.x, renderSize.y);
            glBindTexture(GL_TEXTURE_2D, accumTexture);
            quad.Draw(tonemapShader);
        }
    }

    void Renderer::Present()
    {
        Context& ctx = Context::GetInstance();
        OpenGL::Texture::Active(GL_TEXTURE0);
        OpenGL::Disable(GL_BLEND);
        OpenGL::Disable(GL_DEPTH_TEST);

        if (ctx.scene->dirty || sampleCounter == 1)
        {
            glBindTexture(GL_TEXTURE_2D, pathTraceTextureLowRes);
            quad.Draw(tonemapShader);
        }
        else
        {
            if (ctx.scene->renderOptions.enableDenoiser && denoised)
                glBindTexture(GL_TEXTURE_2D, denoisedTexture);
            else
                glBindTexture(GL_TEXTURE_2D, tileOutputTexture[1 - currentBuffer]);

            // FIXME: RETURN AN IMAGE INSTEAD OF DRAWING IT DIRECTLY
            // THE CODE BELOW EXPORT THE TEXTURE BUFFER AS A PPM FILE AFTER 200
            // SAMPLES

            if (sampleCounter == 200)
            {
                int width = ctx.scene->renderOptions.renderResolution.x;
                int height = ctx.scene->renderOptions.renderResolution.y;
                std::vector<GLubyte> rgba_pixels(width * height * 4);

                if (ctx.scene->renderOptions.enableDenoiser && denoised)
                {
                    glBindTexture(GL_TEXTURE_2D, denoisedTexture);
                    RAY_SUCCESS("Exporting denoised texture");
                    sampleCounter++;
                }
                else
                {
                    glBindTexture(GL_TEXTURE_2D, tileOutputTexture[1 - currentBuffer]);
                    RAY_SUCCESS("Exporting tiled texture");
                }

                glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba_pixels.data());

                std::vector<GLubyte> rgb_pixels(width * height * 3);
                for (int y = 0; y < height; ++y) {
                    for (int x = 0; x < width; ++x) {
                        int rgba_idx = ((height - 1 - y) * width + x) * 4;
                        int rgb_idx = (y * width + x) * 3;
                        rgb_pixels[rgb_idx + 0] = rgba_pixels[rgba_idx + 0]; // R
                        rgb_pixels[rgb_idx + 1] = rgba_pixels[rgba_idx + 1]; // G
                        rgb_pixels[rgb_idx + 2] = rgba_pixels[rgba_idx + 2]; // B
                    }
                }

                std::ofstream file("out.ppm", std::ios::binary);
                if (!file) {
                    throw std::runtime_error("Could not open file for writing");
                }

                file << "P6\n" << width << " " << height << "\n255\n";

                // Write the RGB pixel data
                file.write(reinterpret_cast<char*>(rgb_pixels.data()), rgb_pixels.size());

                // Close the file
                file.close();
            }

            quad.Draw(outputShader);
        }
    }

///////////////////////////////////////////////////////////////////////////////
float Renderer::GetProgress(void)
{
    Context& ctx = Context::GetInstance();

    int maxSpp = ctx.scene->renderOptions.maxSpp;

    if (maxSpp <= 0)
    {
        return (0.f);
    }
    return (sampleCounter * 100.f / maxSpp);
}

///////////////////////////////////////////////////////////////////////////////
int Renderer::GetSampleCount(void)
{
    return (sampleCounter);
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::Update(float secondsElapsed)
{
    RAY_UNUSED(secondsElapsed);
    Context& ctx = Context::GetInstance();
    Renderer::Options& options = ctx.scene->renderOptions;

    if (ctx.scene->dirty == false && options.maxSpp != -1 && sampleCounter >= options.maxSpp)
    {
        return;
    }

    if (ctx.scene->instancesModified)
    {
        glBindTexture(GL_TEXTURE_2D, transformsTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, (sizeof(Mat4x4f) / sizeof(Vec4f)) * ctx.scene->transforms.size(), 1, 0, GL_RGBA, GL_FLOAT, &ctx.scene->transforms[0]);

        glBindTexture(GL_TEXTURE_2D, materialsTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, (sizeof(Material) / sizeof(Vec4f)) * ctx.scene->materials.size(), 1, 0, GL_RGBA, GL_FLOAT, &ctx.scene->materials[0]);

        int index = ctx.scene->bvhTranslator.topLevelIndex;
        int offset = sizeof(Ray::BvhTranslator::Node) * index;
        int size = sizeof(Ray::BvhTranslator::Node) * (ctx.scene->bvhTranslator.nodes.size() - index);
        glBindBuffer(GL_TEXTURE_BUFFER, BVHBuffer);
        glBufferSubData(GL_TEXTURE_BUFFER, offset, size, &ctx.scene->bvhTranslator.nodes[index]);
    }

    if (ctx.scene->envMapModified)
    {
        if (ctx.scene->envMap != nullptr)
        {
            glBindTexture(GL_TEXTURE_2D, envMapTex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, ctx.scene->envMap->width, ctx.scene->envMap->height, 0, GL_RGB, GL_FLOAT, ctx.scene->envMap->img.get());

            glBindTexture(GL_TEXTURE_2D, envMapCDFTex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, ctx.scene->envMap->width, ctx.scene->envMap->height, 0, GL_RED, GL_FLOAT, ctx.scene->envMap->cdf.get());

            GLuint shaderObject;
            pathTraceShader->Use();
            shaderObject = pathTraceShader->GetObject();
            glUniform2f(glGetUniformLocation(shaderObject, "envMapRes"), (float)ctx.scene->envMap->width, (float)ctx.scene->envMap->height);
            glUniform1f(glGetUniformLocation(shaderObject, "envMapTotalSum"), ctx.scene->envMap->totalSum);
            pathTraceShader->StopUsing();

            pathTraceShaderLowRes->Use();
            shaderObject = pathTraceShaderLowRes->GetObject();
            glUniform2f(glGetUniformLocation(shaderObject, "envMapRes"), (float)ctx.scene->envMap->width, (float)ctx.scene->envMap->height);
            glUniform1f(glGetUniformLocation(shaderObject, "envMapTotalSum"), ctx.scene->envMap->totalSum);
            pathTraceShaderLowRes->StopUsing();
        }
    }

        // Denoise image if requested
        if (ctx.scene->renderOptions.enableDenoiser && sampleCounter > 1)
        {
            if (!denoised || (frameCounter % (ctx.scene->renderOptions.denoiserFrameCnt * (numTiles.x * numTiles.y)) == 0))
            {
                glBindTexture(GL_TEXTURE_2D, tileOutputTexture[1 - currentBuffer]);
                glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, denoiserInputFramePtr);

                // Create an Intel Open Image Denoise device
                oidn::DeviceRef device = oidn::newDevice();
                device.commit();

                // Create a denoising filter
                oidn::FilterRef filter = device.newFilter("RT"); // generic ray tracing filter
                filter.setImage("color", denoiserInputFramePtr, oidn::Format::Float3, renderSize.x, renderSize.y, 0, 0, 0);
                filter.setImage("output", frameOutputPtr, oidn::Format::Float3, renderSize.x, renderSize.y, 0, 0, 0);
                filter.set("hdr", false);
                filter.commit();

                // Filter the image
                filter.execute();

                // Check for errors
                const char* errorMessage;
                if (device.getError(errorMessage) != oidn::Error::None)
                    std::cout << "Error: " << errorMessage << std::endl;

                // Copy the denoised data to denoisedTexture
                glBindTexture(GL_TEXTURE_2D, denoisedTexture);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, renderSize.x, renderSize.y, 0, GL_RGB, GL_FLOAT, frameOutputPtr);

                denoised = true;
            }
        }
        else
            denoised = false;

        // If scene was modified then clear out image for re-rendering
        if (ctx.scene->dirty)
        {
            tile.x = -1;
            tile.y = numTiles.y - 1;
            sampleCounter = 1;
            denoised = false;
            frameCounter = 1;

            OpenGL::BindFramebuffer(accumFBO);
            OpenGL::Clear(GL_COLOR_BUFFER_BIT);
        }
        else
        {
            frameCounter++;
            tile.x++;
            if (tile.x >= numTiles.x)
            {
                tile.x = 0;
                tile.y--;
                if (tile.y < 0)
                {
                    tile.x = 0;
                    tile.y = numTiles.y - 1;
                    sampleCounter++;
                    currentBuffer = 1 - currentBuffer;
                }
            }
        }

        pathTraceShader->Use();
        ctx.scene->camera->SetUniforms(pathTraceShader);
        pathTraceShader->Uniform("enableEnvMap", ctx.scene->envMap == nullptr ? false : ctx.scene->renderOptions.enableEnvMap);
        pathTraceShader->Uniform("envMapIntensity", ctx.scene->renderOptions.envMapIntensity);
        pathTraceShader->Uniform("envMapRot", ctx.scene->renderOptions.envMapRot / 360.0f);
        pathTraceShader->Uniform("maxDepth", ctx.scene->renderOptions.maxDepth);
        pathTraceShader->Uniform("tileOffset", Vec2f(tile) * invNumTiles);
        pathTraceShader->Uniform("uniformLightCol", ctx.scene->renderOptions.uniformLightCol);
        pathTraceShader->Uniform("roughnessMollificationAmt", ctx.scene->renderOptions.roughnessMollificationAmt);
        pathTraceShader->Uniform("frameNum", frameCounter);
        pathTraceShader->StopUsing();

        pathTraceShaderLowRes->Use();
        ctx.scene->camera->SetUniforms(pathTraceShaderLowRes);
        pathTraceShaderLowRes->Uniform("enableEnvMap", ctx.scene->envMap == nullptr ? false : ctx.scene->renderOptions.enableEnvMap);
        pathTraceShaderLowRes->Uniform("envMapIntensity", ctx.scene->renderOptions.envMapIntensity);
        pathTraceShaderLowRes->Uniform("envMapRot", ctx.scene->renderOptions.envMapRot / 360.0f);
        pathTraceShaderLowRes->Uniform("maxDepth", ctx.scene->dirty ? 2 : ctx.scene->renderOptions.maxDepth);
        pathTraceShaderLowRes->Uniform("uniformLightCol", ctx.scene->renderOptions.uniformLightCol);
        pathTraceShaderLowRes->Uniform("roughnessMollificationAmt", ctx.scene->renderOptions.roughnessMollificationAmt);
        pathTraceShaderLowRes->StopUsing();

        tonemapShader->Use();
        tonemapShader->Uniform("invSampleCounter", 1.f / sampleCounter);
        tonemapShader->Uniform("enableTonemap", static_cast<int>(ctx.scene->renderOptions.enableTonemap));
        tonemapShader->Uniform("enableAces", static_cast<int>(ctx.scene->renderOptions.enableAces));
        tonemapShader->Uniform("simpleAcesFit", static_cast<int>(ctx.scene->renderOptions.simpleAcesFit));
        tonemapShader->Uniform("backgroundCol", ctx.scene->renderOptions.backgroundCol);
        tonemapShader->StopUsing();
    }
}

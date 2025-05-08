///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Config.hpp"
#include "Core/Renderer.hpp"
#include "Components/Scene.h"
#include "OpenImageDenoise/oidn.hpp"
#include "Errors/Exception.hpp"
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
    : currentBuffer(0)
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

///////////////////////////////////////////////////////////////////////////////
Renderer::~Renderer()
{
    delete[] denoiserInputFramePtr;
    delete[] frameOutputPtr;
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::ActivateTextures(void)
{
    Context& ctx = Context::GetInstance();

    OpenGL::Texture::Active(GL_TEXTURE1);
    BVHTex->Bind();
    OpenGL::Texture::Active(GL_TEXTURE2);
    vertexIndicesTex->Bind();
    OpenGL::Texture::Active(GL_TEXTURE3);
    verticesTex->Bind();
    OpenGL::Texture::Active(GL_TEXTURE4);
    normalsTex->Bind();
    OpenGL::Texture::Active(GL_TEXTURE5);
    materialsTex->Bind();
    OpenGL::Texture::Active(GL_TEXTURE6);
    transformsTex->Bind();
    OpenGL::Texture::Active(GL_TEXTURE7);
    if (lightsTex)
    {
        lightsTex->Bind();
    }
    OpenGL::Texture::Active(GL_TEXTURE8);
    if (!ctx.scene->textures.empty())
    {
        textureMapsArrayTex->Bind();
    }
    OpenGL::Texture::Active(GL_TEXTURE9);
    if (ctx.scene->envMap != nullptr)
    {
        envMapTex->Bind();
    }
    OpenGL::Texture::Active(GL_TEXTURE10);
    if (ctx.scene->envMap != nullptr)
    {
        envMapCDFTex->Bind();
    }
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::InitGPUDataBuffers(void)
{
    Context& ctx = Context::GetInstance();
    UniquePtr<Scene>& scene = ctx.scene;

    OpenGL::PixelStore(GL_PACK_ALIGNMENT, 1);

    BVHBuffer = std::make_unique<OpenGL::Buffer>(GL_TEXTURE_BUFFER);
    BVHBuffer->SetData(
        sizeof(Ray::BvhTranslator::Node) * scene->bvhTranslator.nodes.size(),
        &scene->bvhTranslator.nodes[0],
        GL_STATIC_DRAW
    );
    BVHTex = std::make_unique<OpenGL::TextureBuffer>(BVHBuffer, GL_RGB32F);

    vertexIndicesBuffer = std::make_unique<OpenGL::Buffer>(GL_TEXTURE_BUFFER);
    vertexIndicesBuffer->SetData(
        sizeof(Indices) * scene->vertIndices.size(),
        &scene->vertIndices[0],
        GL_STATIC_DRAW
    );
    vertexIndicesTex = std::make_unique<OpenGL::TextureBuffer>(
        vertexIndicesBuffer, GL_RGB32I
    );

    verticesBuffer = std::make_unique<OpenGL::Buffer>(GL_TEXTURE_BUFFER);
    verticesBuffer->SetData(
        sizeof(Vec4f) * scene->verticesUVX.size(),
        &scene->verticesUVX[0],
        GL_STATIC_DRAW
    );
    verticesTex = std::make_unique<OpenGL::TextureBuffer>(
        verticesBuffer, GL_RGBA32F
    );

    normalsBuffer = std::make_unique<OpenGL::Buffer>(GL_TEXTURE_BUFFER);
    normalsBuffer->SetData(
        sizeof(Vec4f) * scene->normalsUVY.size(),
        &scene->normalsUVY[0],
        GL_STATIC_DRAW
    );
    normalsTex = std::make_unique<OpenGL::TextureBuffer>(
        normalsBuffer, GL_RGBA32F
    );

    materialsTex = std::make_unique<OpenGL::Texture2D>();
    materialsTex->Image2D(
        0, GL_RGBA32F,
        (sizeof(Material) / sizeof(Vec4f)) * scene->materials.size(),
        1, 0, GL_RGBA, GL_FLOAT, &ctx.scene->materials[0]
    );
    materialsTex->SetParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    materialsTex->SetParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    materialsTex->Unbind();

    transformsTex = std::make_unique<OpenGL::Texture2D>();
    transformsTex->Image2D(
        0, GL_RGBA32F,
        (sizeof(Mat4x4f) / sizeof(Vec4f)) * scene->transforms.size(),
        1, 0, GL_RGBA, GL_FLOAT, &ctx.scene->transforms[0]
    );
    transformsTex->SetParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    transformsTex->SetParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    transformsTex->Unbind();

    if (!ctx.scene->lights.empty())
    {
        lightsTex = std::make_unique<OpenGL::Texture2D>();
        lightsTex->Image2D(
            0, GL_RGB32F,
            (sizeof(Light) / sizeof(Vec3f)) * scene->lights.size(),
            1, 0, GL_RGB, GL_FLOAT, &ctx.scene->lights[0]
        );
        lightsTex->SetParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        lightsTex->SetParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        lightsTex->Unbind();
    }

    if (!ctx.scene->textures.empty())
    {
        textureMapsArrayTex = std::make_unique<OpenGL::Texture2DArray>();
        textureMapsArrayTex->Image3D(
            0, GL_RGBA8,
            scene->renderOptions.texArrayWidth,
            scene->renderOptions.texArrayHeight,
            scene->textures.size(), 0, GL_RGBA,
            GL_UNSIGNED_BYTE, &scene->textureMapsArray[0]
        );
        textureMapsArrayTex->SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        textureMapsArrayTex->SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        textureMapsArrayTex->Unbind();
    }

    if (ctx.scene->envMap != nullptr)
    {
        envMapTex = std::make_unique<OpenGL::Texture2D>();
        envMapTex->Image2D(
            0, GL_RGB32F,
            scene->envMap->width, scene->envMap->height,
            0, GL_RGB, GL_FLOAT, scene->envMap->img.get()
        );
        envMapTex->SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        envMapTex->SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        envMapTex->Unbind();

        envMapCDFTex = std::make_unique<OpenGL::Texture2D>();
        envMapCDFTex->Image2D(
            0, GL_R32F,
            scene->envMap->width, scene->envMap->height,
            0, GL_RED, GL_FLOAT, scene->envMap->cdf.get()
        );
        envMapCDFTex->SetParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        envMapCDFTex->SetParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        envMapCDFTex->Unbind();
    }

    ActivateTextures();
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::ResizeRenderer(void)
{
    // Delete denoiser data
    delete[] denoiserInputFramePtr;
    delete[] frameOutputPtr;

    InitFBOs();
    InitShaders();
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::InitFBOs(void)
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
    pathTraceFBO = std::make_unique<OpenGL::FrameBuffer>();
    pathTraceFBO->Bind();

    // Create Texture for FBO
    pathTraceTexture = std::make_unique<OpenGL::Texture2D>();
    pathTraceTexture->Image2D(
        0, GL_RGBA32F, tileWidth, tileHeight,
        0, GL_RGBA, GL_FLOAT, 0
    );
    pathTraceTexture->SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    pathTraceTexture->SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    pathTraceTexture->Unbind();
    pathTraceFBO->Texture2D(pathTraceTexture);

    // Create FBOs for low res preview shader
    pathTraceFBOLowRes = std::make_unique<OpenGL::FrameBuffer>();
    pathTraceFBOLowRes->Bind();

    // Create Texture for FBO
    pathTraceTextureLowRes = std::make_unique<OpenGL::Texture2D>();
    pathTraceTextureLowRes->Image2D(
        0, GL_RGBA32F, windowSize.x * pixelRatio, windowSize.y * pixelRatio,
        0, GL_RGBA, GL_FLOAT, 0
    );
    pathTraceTextureLowRes->SetParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    pathTraceTextureLowRes->SetParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    pathTraceTextureLowRes->SetParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    pathTraceTextureLowRes->SetParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    pathTraceTextureLowRes->Unbind();
    pathTraceFBOLowRes->Texture2D(pathTraceTextureLowRes);

    // Create FBOs for accum buffer
    accumFBO = std::make_unique<OpenGL::FrameBuffer>();
    accumFBO->Bind();

    // Create Texture for FBO
    accumTexture = std::make_unique<OpenGL::Texture2D>();
    accumTexture->Image2D(
        0, GL_RGBA32F, renderSize.x, renderSize.y,
        0, GL_RGBA, GL_FLOAT, 0
    );
    accumTexture->SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    accumTexture->SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    accumTexture->Unbind();
    accumFBO->Texture2D(accumTexture);

    // Create FBOs for tile output shader
    outputFBO = std::make_unique<OpenGL::FrameBuffer>();
    outputFBO->Bind();

    // Create Texture for FBO
    tileOutputTexture[0] = std::make_unique<OpenGL::Texture2D>();
    tileOutputTexture[0]->Image2D(
        0, GL_RGBA32F, renderSize.x, renderSize.y,
        0, GL_RGBA, GL_FLOAT, 0
    );
    tileOutputTexture[0]->SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    tileOutputTexture[0]->SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    tileOutputTexture[0]->Unbind();

    tileOutputTexture[1] = std::make_unique<OpenGL::Texture2D>();
    tileOutputTexture[1]->Image2D(
        0, GL_RGBA32F, renderSize.x, renderSize.y,
        0, GL_RGBA, GL_FLOAT, 0
    );
    tileOutputTexture[1]->SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    tileOutputTexture[1]->SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    tileOutputTexture[1]->Unbind();

    outputFBO->Texture2D(tileOutputTexture[currentBuffer]);

    // For Denoiser
    denoiserInputFramePtr = new Vec3f[renderSize.x * renderSize.y];
    frameOutputPtr = new Vec3f[renderSize.x * renderSize.y];

    denoisedTexture = std::make_unique<OpenGL::Texture2D>();
    denoisedTexture->Image2D(
        0, GL_RGB32F, renderSize.x, renderSize.y,
        0, GL_RGB, GL_FLOAT, 0
    );
    denoisedTexture->SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    denoisedTexture->SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    denoisedTexture->Unbind();

    RAY_INFO("Window Resolution: " << windowSize);
    RAY_INFO("Render Resolution: " << renderSize);
    RAY_INFO("Preview Resolution: " << Vec2i(Vec2f(windowSize) * pixelRatio));
    RAY_INFO("Tile Size: " << Vec2i(tileWidth, tileHeight));
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::ReloadShaders(void)
{
    pathTraceShader.reset();
    pathTraceShaderLowRes.reset();
    outputShader.reset();
    tonemapShader.reset();

    InitShaders();
}

///////////////////////////////////////////////////////////////////////////////
String Renderer::GetPathtraceShaderOptions(void) const
{
    Context& ctx = Context::GetInstance();
    UniquePtr<Scene>& scene = ctx.scene;
    Renderer::Options& options = scene->renderOptions;
    String defines;

    if (options.enableEnvMap && scene->envMap != nullptr)
    {
        defines += DEFINE_OPTIONS(RENDERER_ENABLE_ENVMAP);
    }

    if (!scene->lights.empty())
    {
        defines += DEFINE_OPTIONS(RENDERER_ENABLE_LIGHTS);
    }

    if (options.enableRR)
    {
        defines += DEFINE_OPTIONS(RENDERER_ENABLE_RR);
        defines += DEFINE_OPTIONS(
            + (RENDERER_OPTIONS_RRDEPTH + std::to_string(options.RRDepth)) +);
    }

    if (options.enableUniformLight)
    {
        defines += DEFINE_OPTIONS(RENDERER_ENABLE_UNIFORM);
    }

    if (options.openglNormalMap)
    {
        defines += DEFINE_OPTIONS(RENDERER_ENABLE_OPENGLNORMAL);
    }

    if (options.hideEmitters)
    {
        defines += DEFINE_OPTIONS(RENDERER_ENABLE_HIDEEMITTERS);
    }

    if (options.enableBackground)
    {
        defines += DEFINE_OPTIONS(RENDERER_ENABLE_BACKGROUND);
    }

    if (options.transparentBackground)
    {
        defines += DEFINE_OPTIONS(RENDERER_ENABLE_TRANSPARENT_BG);
    }

    for (const auto& material : scene->materials)
    {
        if (static_cast<int>(material.alphaMode) != Material::OPAQUE)
        {
            defines += DEFINE_OPTIONS(RENDERER_ENABLE_ALPHA_TEST);
            break;
        }
    }

    if (options.enableRoughnessMollification)
    {
        defines += DEFINE_OPTIONS(RENDERER_ENABLE_ROUGHNESS_MOLL);
    }

    for (const auto& material : scene->materials)
    {
        if (static_cast<int>(material.mediumType) != Material::NONE)
        {
            defines += DEFINE_OPTIONS(RENDERER_ENABLE_MEDIUM);
            break;
        }
    }

    if (options.enableVolumeMIS)
    {
        defines += DEFINE_OPTIONS(RENDERER_ENABLE_VOLUME_MIS);
    }

    return (defines);
}

///////////////////////////////////////////////////////////////////////////////
String Renderer::GetTonemapShaderOptions(void) const
{
    Context& ctx = Context::GetInstance();
    Renderer::Options& options = ctx.scene->renderOptions;
    String defines;

    if (options.enableBackground)
    {
        defines += DEFINE_OPTIONS(RENDERER_ENABLE_BACKGROUND);
    }

    if (options.transparentBackground)
    {
        defines += DEFINE_OPTIONS(RENDERER_ENABLE_TRANSPARENT_BG);
    }

    return (defines);
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::InsertDefinitions(String& source, const String& defines)
{
    Uint64 index = source.find("#version");

    if (index != String::npos)
    {
        index = source.find("\n", index);
    }
    else
    {
        index = 0;
    }

    source.insert(index + 1, defines);
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::InitShaders(void)
{
    Context& ctx = Context::GetInstance();
    Path shadersDirectory = ctx.GetShaderPath();

    String vertexSrc = Shader::Source(
        shadersDirectory / "Vertex/Vertex.glsl");
    String pathTraceSrc = Shader::Source(
        shadersDirectory / "Lighting/Tile.glsl");
    String pathTraceLowResSrc = Shader::Source(
        shadersDirectory / "Stages/Preview.glsl");
    String tonemapSrc = Shader::Source(
        shadersDirectory / "Lighting/Tonemap.glsl");

    String pathtraceDefines = GetPathtraceShaderOptions();
    String tonemapDefines = GetTonemapShaderOptions();

    if (!pathtraceDefines.empty())
    {
        InsertDefinitions(pathTraceSrc, pathtraceDefines);
        InsertDefinitions(pathTraceLowResSrc, pathtraceDefines);
    }

    if (!tonemapDefines.empty())
    {
        InsertDefinitions(tonemapSrc, tonemapDefines);
    }

    pathTraceShader = std::make_unique<Shader>(
        vertexSrc, pathTraceSrc);
    pathTraceShaderLowRes = std::make_unique<Shader>(
        vertexSrc, pathTraceLowResSrc);
    tonemapShader = std::make_unique<Shader>(
        vertexSrc, tonemapSrc);

    if (!outputShader)
    {
        String outputSrc = Shader::Source(
            shadersDirectory / "Stages/Output.glsl");
        outputShader = std::make_unique<Shader>(vertexSrc, outputSrc);
    }

    InitializeUniforms(pathTraceShader);
    InitializeUniforms(pathTraceShaderLowRes);
}

///////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////
void Renderer::ExportRender(const Path& destination)
{
    Context& ctx = Context::GetInstance();

    if (ctx.scene->dirty || sampleCounter == 1)
    {
        RAY_ERROR("Cannot export a dirty render");
        return;
    }

    int width = ctx.scene->renderOptions.renderResolution.x;
    int height = ctx.scene->renderOptions.renderResolution.y;
    Vector<GLubyte> rgba_pixels(width * height * 4);

    if (ctx.scene->renderOptions.enableDenoiser && denoised)
    {
        denoisedTexture->Bind();
        RAY_SUCCESS("Exporting denoised texture");
        sampleCounter++;
    }
    else
    {
        tileOutputTexture[1 - currentBuffer]->Bind();
        RAY_SUCCESS("Exporting tiled texture");
    }

    glGetTexImage(
        GL_TEXTURE_2D, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, rgba_pixels.data()
    );

    Vector<GLubyte> rgb_pixels(width * height * 3);
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int rgba_idx = ((height - 1 - y) * width + x) * 4;
            int rgb_idx = (y * width + x) * 3;
            rgb_pixels[rgb_idx + 0] = rgba_pixels[rgba_idx + 0];
            rgb_pixels[rgb_idx + 1] = rgba_pixels[rgba_idx + 1];
            rgb_pixels[rgb_idx + 2] = rgba_pixels[rgba_idx + 2];
        }
    }

    std::ofstream file(destination, std::ios::binary);
    if (!file)
    {
        throw Exception("Could not open file for writing");
    }

    file << "P6\n" << width << " " << height << "\n255\n";

    file.write(reinterpret_cast<char*>(rgb_pixels.data()), rgb_pixels.size());

    file.close();
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::Render(void)
{
    Context& ctx = Context::GetInstance();

    if (!ctx.scene->CanUpdate(sampleCounter))
    {
        return;
    }

    OpenGL::Disable(GL_BLEND);
    OpenGL::Disable(GL_DEPTH_TEST);
    OpenGL::Texture::Active(GL_TEXTURE0);

    if (ctx.scene->dirty)
    {
        pathTraceFBOLowRes->Bind();
        OpenGL::Viewport(Vec2i(0), Vec2i(Vec2f(windowSize) * pixelRatio));
        quad.Draw(pathTraceShaderLowRes);

        ctx.scene->instancesModified = false;
        ctx.scene->dirty = false;
        ctx.scene->envMapModified = false;
    }
    else
    {
        pathTraceFBO->Bind();
        OpenGL::Viewport(Vec2i(0), Vec2i(tileWidth, tileHeight));
        accumTexture->Bind();
        quad.Draw(pathTraceShader);

        accumFBO->Bind();
        OpenGL::Viewport(
            Vec2i(tileWidth * tile.x, tileHeight * tile.y),
            Vec2i(tileWidth, tileHeight)
        );
        pathTraceTexture->Bind();
        quad.Draw(outputShader);

        outputFBO->Bind();
        outputFBO->Texture2D(tileOutputTexture[currentBuffer]);
        OpenGL::Viewport(Vec2i(0), renderSize);
        accumTexture->Bind();
        quad.Draw(tonemapShader);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::Present(void)
{
    Context& ctx = Context::GetInstance();
    OpenGL::Texture::Active(GL_TEXTURE0);
    OpenGL::Disable(GL_BLEND);
    OpenGL::Disable(GL_DEPTH_TEST);

    if (ctx.scene->dirty || sampleCounter == 1)
    {
        pathTraceTextureLowRes->Bind();
        quad.Draw(tonemapShader);
    }
    else
    {
        if (ctx.scene->renderOptions.enableDenoiser && denoised)
        {
            denoisedTexture->Bind();
        }
        else
        {
            tileOutputTexture[1 - currentBuffer]->Bind();
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
void Renderer::UpdateUniforms(void)
{
    Context& ctx = Context::GetInstance();

    pathTraceShader->Use();
    ctx.scene->camera->SetUniforms(pathTraceShader);
    pathTraceShader->Uniform("enableEnvMap", ctx.scene->envMap == nullptr ?
        false : ctx.scene->renderOptions.enableEnvMap);
    pathTraceShader->Uniform("envMapIntensity",
        ctx.scene->renderOptions.envMapIntensity);
    pathTraceShader->Uniform("envMapRot",
        ctx.scene->renderOptions.envMapRot / 360.0f);
    pathTraceShader->Uniform("maxDepth", ctx.scene->renderOptions.maxDepth);
    pathTraceShader->Uniform("tileOffset", Vec2f(tile) * invNumTiles);
    pathTraceShader->Uniform("uniformLightCol",
        ctx.scene->renderOptions.uniformLightCol);
    pathTraceShader->Uniform("roughnessMollificationAmt",
        ctx.scene->renderOptions.roughnessMollificationAmt);
    pathTraceShader->Uniform("frameNum", frameCounter);
    pathTraceShader->StopUsing();

    pathTraceShaderLowRes->Use();
    ctx.scene->camera->SetUniforms(pathTraceShaderLowRes);
    pathTraceShaderLowRes->Uniform("enableEnvMap", ctx.scene->envMap == nullptr
        ? false : ctx.scene->renderOptions.enableEnvMap);
    pathTraceShaderLowRes->Uniform("envMapIntensity",
        ctx.scene->renderOptions.envMapIntensity);
    pathTraceShaderLowRes->Uniform("envMapRot",
        ctx.scene->renderOptions.envMapRot / 360.0f);
    pathTraceShaderLowRes->Uniform("maxDepth",
        ctx.scene->dirty ? 2 : ctx.scene->renderOptions.maxDepth);
    pathTraceShaderLowRes->Uniform("uniformLightCol",
        ctx.scene->renderOptions.uniformLightCol);
    pathTraceShaderLowRes->Uniform("roughnessMollificationAmt",
        ctx.scene->renderOptions.roughnessMollificationAmt);
    pathTraceShaderLowRes->StopUsing();

    tonemapShader->Use();
    tonemapShader->Uniform("invSampleCounter", 1.f / sampleCounter);
    tonemapShader->Uniform("enableTonemap",
        static_cast<int>(ctx.scene->renderOptions.enableTonemap));
    tonemapShader->Uniform("enableAces",
        static_cast<int>(ctx.scene->renderOptions.enableAces));
    tonemapShader->Uniform("simpleAcesFit",
        static_cast<int>(ctx.scene->renderOptions.simpleAcesFit));
    tonemapShader->Uniform("backgroundCol",
        ctx.scene->renderOptions.backgroundCol);
    tonemapShader->StopUsing();
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::UpdateSceneInstance(void)
{
    Context& ctx = Context::GetInstance();

    if (!ctx.scene->instancesModified)
    {
        return;
    }

    transformsTex->Image2D(
        0, GL_RGBA32F,
        (sizeof(Mat4x4f) / sizeof(Vec4f)) * ctx.scene->transforms.size(),
        1, 0, GL_RGBA, GL_FLOAT, &ctx.scene->transforms[0]
    );
    materialsTex->Image2D(
        0, GL_RGBA32F,
        (sizeof(Material) / sizeof(Vec4f)) * ctx.scene->materials.size(),
        1, 0, GL_RGBA, GL_FLOAT, &ctx.scene->materials[0]
    );

    int index = ctx.scene->bvhTranslator.topLevelIndex;
    int offset = sizeof(Ray::BvhTranslator::Node) * index;
    int size = sizeof(Ray::BvhTranslator::Node) *
        (ctx.scene->bvhTranslator.nodes.size() - index);
    BVHBuffer->SetSubData(offset, size,
        &ctx.scene->bvhTranslator.nodes[index]);
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::UpdateEnvMap(void)
{
    Context& ctx = Context::GetInstance();

    if (!ctx.scene->envMapModified || ctx.scene->envMap == nullptr)
    {
        return;
    }

    envMapTex->Image2D(
        0, GL_RGB32F,
        ctx.scene->envMap->width, ctx.scene->envMap->height,
        0, GL_RGB, GL_FLOAT, ctx.scene->envMap->img.get()
    );
    envMapCDFTex->Image2D(
        0, GL_R32F,
        ctx.scene->envMap->width, ctx.scene->envMap->height,
        0, GL_RED, GL_FLOAT, ctx.scene->envMap->cdf.get()
    );

    pathTraceShader->Use();
    pathTraceShader->Uniform("envMapRes", Vec2f(
        ctx.scene->envMap->width, ctx.scene->envMap->height));
    pathTraceShader->Uniform("envMapTotalSum", ctx.scene->envMap->totalSum);
    pathTraceShader->StopUsing();

    pathTraceShaderLowRes->Use();
    pathTraceShaderLowRes->Uniform("envMapRes", Vec2f(
        ctx.scene->envMap->width, ctx.scene->envMap->height));
    pathTraceShaderLowRes->Uniform("envMapTotalSum",
        ctx.scene->envMap->totalSum);
    pathTraceShaderLowRes->StopUsing();
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::DenoiseRender(void)
{
    Context& ctx = Context::GetInstance();

    if (
        denoised ||
        (frameCounter % (
            ctx.scene->renderOptions.denoiserFrameCnt *
            (numTiles.x * numTiles.y)
        ) != 0)
    )
    {
        return;
    }

    tileOutputTexture[1 - currentBuffer]->Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, denoiserInputFramePtr);

    oidn::DeviceRef device = oidn::newDevice();
    device.commit();

    Uint64 imageWidth = static_cast<Uint64>(renderSize.x);
    Uint64 imageHeight = static_cast<Uint64>(renderSize.y);
    Uint64 numChannels = 3;
    Uint64 size = imageWidth * imageHeight * numChannels * sizeof(float);

    oidn::BufferRef colorBuffer = device.newBuffer(size);
    oidn::BufferRef outputBuffer = device.newBuffer(size);

    colorBuffer.write(0, size, denoiserInputFramePtr);

    oidn::FilterRef filter = device.newFilter("RT");

    filter.setImage(
        "color", colorBuffer, oidn::Format::Float3,
        imageWidth, imageHeight, 0, numChannels * sizeof(float),
        imageWidth * numChannels * sizeof(float)
    );
    filter.setImage(
        "output", outputBuffer, oidn::Format::Float3,
        imageWidth, imageHeight, 0, numChannels * sizeof(float),
        imageWidth * numChannels * sizeof(float)
    );
    filter.set("hdr", false);

    filter.commit();
    filter.execute();

    const char* errorMessage;
    if (device.getError(errorMessage) != oidn::Error::None)
    {
        std::cout << "OIDN Error: " << errorMessage << std::endl;
    }
    else
    {
        outputBuffer.read(0, size, frameOutputPtr);
        denoisedTexture->Image2D(
            0, GL_RGB32F, renderSize.x, renderSize.y,
            0, GL_RGB, GL_FLOAT, frameOutputPtr
        );
    }

    denoised = true;
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::Update(float secondsElapsed)
{
    RAY_UNUSED(secondsElapsed);
    Context& ctx = Context::GetInstance();

    if (!ctx.scene->CanUpdate(sampleCounter))
    {
        return;
    }

    UpdateSceneInstance();
    UpdateEnvMap();

    if (ctx.scene->renderOptions.enableDenoiser && sampleCounter > 1)
    {
        DenoiseRender();
    }
    else
    {
        denoised = false;
    }

    if (ctx.scene->dirty)
    {
        tile.x = -1;
        tile.y = numTiles.y - 1;
        sampleCounter = 1;
        denoised = false;
        frameCounter = 1;

        accumFBO->Bind();
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

    UpdateUniforms();
}

} // namespace Ray

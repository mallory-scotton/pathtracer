///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Config.hpp"
#include "Renderer/Scene.hpp"
#include "Resources/Shader.hpp"
#include "Renderer/Quad.hpp"
#include "Utils/OpenGL.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class Renderer
{
public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct Options
    {
        Vec2i renderResolution{1280, 720};          //<!
        Vec2i windowResolution{1280, 720};          //<!
        Color uniformLightColor{.3f, .3f, .3f};     //<!
        Color backgroundColor{1.f, 1.f, 1.f};       //<!
        int tileWidth{100};                         //<!
        int tileHeight{100};                        //<!
        int maxDepth{2};                            //<!
        int maxSPP{-1};                             //<!
        int RRDepth{2};                             //<!
        int textureArrayWidth{2048};                //<!
        int textureArrayHeight{2048};               //<!
        int denoiserFrameCount{20};                 //<!
        bool enableRR{true};                        //<!
        bool enableDenoiser{false};                 //<!
        bool enableTonemap{true};                   //<!
        bool enableAces{false};                     //<!
        bool openglNormalMap{true};                 //<!
        bool enableEnvMap{false};                   //<!
        bool enableUniformLight{false};             //<!
        bool hideEmitters{false};                   //<!
        bool enableBackground{false};               //<!
        bool transparentBackground{false};          //<!
        bool independantRenderSize{false};          //<!
        bool enableRoughnessMollification{false};   //<!
        bool enableVolumeMIS{false};                //<!
        float envMapIntensity{1.f};                 //<!
        float envMapRot{0.f};                       //<!
        float roughnessMollificationAmount{0.f};    //<!
    };

private:
    ///////////////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////////////
    GLuint mBVHBuffer{0};                       //<!
    GLuint mBVHTexture{0};                      //<!
    GLuint mVertexIndicesBuffer{0};             //<!
    GLuint mVertexIndicesTexture{0};            //<!
    GLuint mVerticesBuffer{0};                  //<!
    GLuint mVerticesTexture{0};                 //<!
    GLuint mNormalsBuffer{0};                   //<!
    GLuint mNormalsTexture{0};                  //<!
    GLuint mMaterialsTexture{0};                //<!
    GLuint mTransformsTexture{0};               //<!
    GLuint mLightsTexture{0};                   //<!
    GLuint mTextureMapsArrayTexture{0};         //<!
    GLuint mEnvMapTexture{0};                   //<!
    GLuint mEnvMapCDFTexture{0};                //<!

    GLuint mPathTraceFBO{0};                    //<!
    GLuint mPathTraceFBOLowRes{0};              //<!
    GLuint mAccumulationFBO{0};                 //<!
    GLuint mOutputFBO{0};                       //<!

    GLuint mPathTraceTextureLowRes{0};          //<!
    GLuint mPathTraceTexture{0};                //<!
    GLuint mAccumulationTexture{0};             //<!
    GLuint mTileOutputTexture[2]{0};            //<!
    GLuint mDenoisedTexture{0};                 //<!

    SharedPtr<Scene> mScene;                    //<!

    Quad mQuad;                                 //<!
    UniquePtr<Shader> mPathTraceShader;         //<!
    UniquePtr<Shader> mPathTraceShaderLowRes;   //<!
    UniquePtr<Shader> mOutputShader;            //<!
    UniquePtr<Shader> mTonemapShader;           //<!

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    Renderer();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    ~Renderer();

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void InitGPUDataBuffers(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void InitFBOs(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void InitShaders(void);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Render(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Update(float deltaSeconds);
};

} // namespace Ray

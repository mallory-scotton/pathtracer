///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Config.hpp"
#include "Maths.hpp"
#include "Resources/Shader.hpp"
#include "Renderer/Quad.hpp"
#include "Utils/OpenGL.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
class Scene;

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
    class Options
    {
    public:
        ///////////////////////////////////////////////////////////////////////
        //
        ///////////////////////////////////////////////////////////////////////
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
        bool simpleAcesFit{false};                  //<!
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

    public:
        ///////////////////////////////////////////////////////////////////////
        /// \brief
        ///
        ///////////////////////////////////////////////////////////////////////
        Options(void) = default;

        ///////////////////////////////////////////////////////////////////////
        /// \brief
        ///
        /// \param props
        ///
        ///////////////////////////////////////////////////////////////////////
        Options(const Map<String, Vector<String>>& props);

    public:
        ///////////////////////////////////////////////////////////////////////
        /// \brief
        ///
        /// \param scene
        ///
        /// \return
        ///
        ///////////////////////////////////////////////////////////////////////
        String GetPathTraceShaderDefinitions(
            const SharedPtr<Scene>& scene
        ) const;

        ///////////////////////////////////////////////////////////////////////
        /// \brief
        ///
        /// \return
        ///
        ///////////////////////////////////////////////////////////////////////
        String GetTonemapShaderDefinitions(void) const;
    };

private:
    ///////////////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////////////
    GLuint mBVHBuffer{0};                           //<!
    GLuint mBVHTexture{0};                          //<!
    GLuint mVertexIndicesBuffer{0};                 //<!
    GLuint mVertexIndicesTexture{0};                //<!
    GLuint mVerticesBuffer{0};                      //<!
    GLuint mVerticesTexture{0};                     //<!
    GLuint mNormalsBuffer{0};                       //<!
    GLuint mNormalsTexture{0};                      //<!
    GLuint mMaterialsTexture{0};                    //<!
    GLuint mTransformsTexture{0};                   //<!
    GLuint mLightsTexture{0};                       //<!
    GLuint mTextureMapsArrayTexture{0};             //<!
    GLuint mEnvMapTexture{0};                       //<!
    GLuint mEnvMapCDFTexture{0};                    //<!

    GLuint mPathTraceFBO{0};                        //<!
    GLuint mPathTraceFBOLowRes{0};                  //<!
    GLuint mAccumulationFBO{0};                     //<!
    GLuint mOutputFBO{0};                           //<!

    GLuint mPathTraceTextureLowRes{0};              //<!
    GLuint mPathTraceTexture{0};                    //<!
    GLuint mAccumulationTexture{0};                 //<!
    GLuint mTileOutputTexture[2]{0};                //<!
    GLuint mDenoisedTexture{0};                     //<!

    SharedPtr<Scene> mScene;                        //<!
    Options mOptions;                               //<!

    Quad mQuad;                                     //<!
    UniquePtr<Shader> mPathTraceShader;             //<!
    UniquePtr<Shader> mPathTraceShaderLowRes;       //<!
    UniquePtr<Shader> mOutputShader;                //<!
    UniquePtr<Shader> mTonemapShader;               //<!

    Vec2i mRenderSize;                              //<!
    Vec2i mWindowSize;                              //<!
    Vec2i mTile;                                    //<!
    Vec2i mTileCount;                               //<!
    Vec2f mInvTileCount;                            //<!
    int mTileWidth;                                 //<!
    int mTileHeight;                                //<!
    int mCurrentBuffer;                             //<!
    int mFrameCounter;                              //<!
    int mSampleCounter;                             //<!
    float mPixelRatio{.25f};                        //<!

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param scene
    ///
    ///////////////////////////////////////////////////////////////////////////
    Renderer(SharedPtr<Scene> scene);

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
    void InitPathTraceFBO(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void InitPathTraceLowResFBO(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void InitAccumulationFBO(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void InitOutputFBO(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void InitShaders(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void InitUniforms(UniquePtr<Shader>& shader);

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
    void Present(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void DrawToScreen(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param deltaSeconds
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Update(float deltaSeconds);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void ReloadShaders(void);
};

} // namespace Ray

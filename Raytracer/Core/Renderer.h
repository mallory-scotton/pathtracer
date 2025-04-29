// FIXME: THE WHOLE FILE

#pragma once

#include <vector>
#include "Config.hpp"
#include "Core/Quad.hpp"
#include "Core/Shader.hpp"
#include "Maths/Vec2.hpp"
#include "Maths/Vec3.hpp"

namespace Ray
{
    struct RenderOptions
    {
        RenderOptions()
        {
            renderResolution = Vec2i(1280, 720);
            windowResolution = Vec2i(1280, 720);
            uniformLightCol = Vec3f(0.3f);
            backgroundCol = Vec3f(1.0f);
            tileWidth = 100;
            tileHeight = 100;
            maxDepth = 2;
            maxSpp = -1;
            RRDepth = 2;
            texArrayWidth = 2048;
            texArrayHeight = 2048;
            denoiserFrameCnt = 20;
            enableRR = true;
            enableDenoiser = false;
            enableTonemap = true;
            enableAces = false;
            openglNormalMap = true;
            enableEnvMap = false;
            enableUniformLight = false;
            hideEmitters = false;
            enableBackground = false;
            transparentBackground = false;
            independentRenderSize = false;
            enableRoughnessMollification = false;
            enableVolumeMIS = false;
            envMapIntensity = 1.0f;
            envMapRot = 0.0f;
            roughnessMollificationAmt = 0.0f;
        }

        Vec2i renderResolution;
        Vec2i windowResolution;
        Vec3f uniformLightCol;
        Vec3f backgroundCol;
        int tileWidth;
        int tileHeight;
        int maxDepth;
        int maxSpp;
        int RRDepth;
        int texArrayWidth;
        int texArrayHeight;
        int denoiserFrameCnt;
        bool enableRR;
        bool enableDenoiser;
        bool enableTonemap;
        bool enableAces;
        bool simpleAcesFit;
        bool openglNormalMap;
        bool enableEnvMap;
        bool enableUniformLight;
        bool hideEmitters;
        bool enableBackground;
        bool transparentBackground;
        bool independentRenderSize;
        bool enableRoughnessMollification;
        bool enableVolumeMIS;
        float envMapIntensity;
        float envMapRot;
        float roughnessMollificationAmt;
    };

    class Scene;

    class Renderer
    {
    protected:
        Scene* scene;
        Quad* quad;

        // Opengl buffer objects and textures for storing scene data on the GPU
        GLuint BVHBuffer;
        GLuint BVHTex;
        GLuint vertexIndicesBuffer;
        GLuint vertexIndicesTex;
        GLuint verticesBuffer;
        GLuint verticesTex;
        GLuint normalsBuffer;
        GLuint normalsTex;
        GLuint materialsTex;
        GLuint transformsTex;
        GLuint lightsTex;
        GLuint textureMapsArrayTex;
        GLuint envMapTex;
        GLuint envMapCDFTex;

        // FBOs
        GLuint pathTraceFBO;
        GLuint pathTraceFBOLowRes;
        GLuint accumFBO;
        GLuint outputFBO;

        // Shaders
        std::string shadersDirectory;
        UniquePtr<Shader> pathTraceShader;
        UniquePtr<Shader> pathTraceShaderLowRes;
        UniquePtr<Shader> outputShader;
        UniquePtr<Shader> tonemapShader;

        // Render textures
        GLuint pathTraceTextureLowRes;
        GLuint pathTraceTexture;
        GLuint accumTexture;
        GLuint tileOutputTexture[2];
        GLuint denoisedTexture;

        // Render resolution and window resolution
        Vec2i renderSize;
        Vec2i windowSize;

        // Variables to track rendering status
        Vec2i tile;
        Vec2i numTiles;
        Vec2f invNumTiles;
        int tileWidth;
        int tileHeight;
        int currentBuffer;
        int frameCounter;
        int sampleCounter;
        float pixelRatio;

        // Denoiser output
        Vec3f* denoiserInputFramePtr;
        Vec3f* frameOutputPtr;
        bool denoised;

        bool initialized;

    public:
        Renderer(Scene* scene, const std::string& shadersDirectory);
        ~Renderer();

        void ResizeRenderer();
        void ReloadShaders();
        void Render();
        void Present();
        void Update(float secondsElapsed);
        float GetProgress();
        int GetSampleCount();
        void GetOutputBuffer(unsigned char**, int& w, int& h);

    private:
        void InitGPUDataBuffers();
        void InitFBOs();
        void InitShaders();
        void InitializeUniforms(UniquePtr<Shader>& shader);
    };
}

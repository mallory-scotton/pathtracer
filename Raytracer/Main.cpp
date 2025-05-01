// FIXME: THE WHOLE FILE
#include <SDL2/SDL.h>
#include <math.h>
#include <time.h>

#include <string>

#include "Core/Renderer.h"
#include "Components/Scene.h"
#include "GL/gl3w.h"
#include "ImGui/tinydir.h"
#include "Loaders/GLTFLoader.h"
#include "Loaders/Loader.hpp"
#include "Core/Context.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "ImGui/stb_image.h"
#include "ImGui/stb_image_write.h"

using namespace std;
using namespace Ray;

Scene* scene = nullptr;
Renderer* renderer = nullptr;

Vector<String> sceneFiles;
Vector<String> envMaps;

float mouseSensitivity = 0.01f;
bool keyPressed = false;
int sampleSceneIdx = 0;
int selectedInstance = 0;
double lastTime = SDL_GetTicks();
int envMapIdx = 0;
bool done = false;

Context& ctx = Context::GetInstance();

String shadersDir = "Shaders/";
String assetsDir = "Assets/";
String envMapDir = "Assets/HDR/";

RenderOptions renderOptions;

struct LoopData {
    SDL_Window* mWindow = nullptr;
    SDL_GLContext mGLContext = nullptr;
};

void GetSceneFiles()
{
    tinydir_dir dir;
    int i;
    tinydir_open_sorted(&dir, assetsDir.c_str());

    for (i = 0; i < dir.n_files; i++) {
        tinydir_file file;
        tinydir_readfile_n(&dir, &file, i);

        std::string ext = std::string(file.extension);
        if (ext == "scene" || ext == "gltf" || ext == "glb") {
            sceneFiles.push_back(assetsDir + std::string(file.name));
        }
    }

    tinydir_close(&dir);
}

void GetEnvMaps() {
    tinydir_dir dir;
    int i;
    tinydir_open_sorted(&dir, envMapDir.c_str());

    for (i = 0; i < dir.n_files; i++) {
        tinydir_file file;
        tinydir_readfile_n(&dir, &file, i);

        std::string ext = std::string(file.extension);
        if (ext == "hdr") {
            envMaps.push_back(envMapDir + std::string(file.name));
        }
    }

    tinydir_close(&dir);
}

void LoadScene(std::string sceneName) {
    delete scene;
    scene = new Scene();
    std::string ext = sceneName.substr(sceneName.find_last_of(".") + 1);

    bool success = false;
    Mat4x4f xform;

    success = Loader::LoadScene("Scenes/Teapot.scene", scene, renderOptions);

    // if (ext == "scene")
    //    success = LoadSceneFromFile(sceneName, scene, renderOptions);
    // else if (ext == "gltf")
    //    success = LoadGLTF(sceneName, scene, renderOptions, xform, false);
    // else if (ext == "glb")
    //    success = LoadGLTF(sceneName, scene, renderOptions, xform, true);

    if (!success) {
        printf("Unable to load scene\n");
        exit(0);
    }

    // loadCornellTestScene(scene, renderOptions);
    selectedInstance = 0;

    // Add a default HDR if there are no lights in the scene
    if (!scene->envMap && !envMaps.empty()) {
        scene->AddEnvMap(envMaps[envMapIdx]);
        renderOptions.enableEnvMap = scene->lights.empty() ? true : false;
        renderOptions.envMapIntensity = 1.5f;
    }

    scene->renderOptions = renderOptions;
}

bool InitRenderer() {
    delete renderer;
    renderer = new Renderer(scene, shadersDir);
    return true;
}

void SaveFrame(const std::string filename) {
    unsigned char* data = nullptr;
    int w, h;
    renderer->GetOutputBuffer(&data, w, h);
    stbi_flip_vertically_on_write(true);
    stbi_write_png(filename.c_str(), w, h, 4, data, w * 4);
    printf("Frame saved: %s\n", filename.c_str());
    delete[] data;
}

void Render() {
    renderer->Render();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, renderOptions.windowResolution.x,
               renderOptions.windowResolution.y);
    renderer->Present();
}

void Update(float secondsElapsed) {
    renderer->Update(secondsElapsed);
}

void MainLoop(void* arg) {
    LoopData& loopdata = *(LoopData*)arg;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            done = true;
        }
        if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                renderOptions.windowResolution =
                    Vec2i(event.window.data1, event.window.data2);
                int w, h;
                SDL_GL_GetDrawableSize(loopdata.mWindow, &w, &h);
                renderOptions.windowResolution.x = w;
                renderOptions.windowResolution.y = h;

                if (!renderOptions.independentRenderSize)
                    renderOptions.renderResolution =
                        renderOptions.windowResolution;

                scene->renderOptions = renderOptions;
                renderer->ResizeRenderer();
            }

            if (event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(loopdata.mWindow)) {
                done = true;
            }
        }
    }

    double presentTime = SDL_GetTicks();
    Update((float)(presentTime - lastTime));
    lastTime = presentTime;
    glClearColor(0., 0., 0., 0.);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    Render();
    SDL_GL_SwapWindow(loopdata.mWindow);
}

int main(int argc, char** argv)
{
    srand((unsigned int)time(0));

    std::string sceneFile;

    for (int i = 1; i < argc; ++i) {
        const std::string arg(argv[i]);
        if (arg == "-s" || arg == "--scene") {
            sceneFile = argv[++i];
        } else if (arg[0] == '-') {
            printf("Unknown option %s \n'", arg.c_str());
            exit(0);
        }
    }

    if (!sceneFile.empty()) {
        scene = new Scene();
        GetEnvMaps();
        LoadScene(sceneFile);
    } else {
        GetSceneFiles();
        GetEnvMaps();
        LoadScene(sceneFiles[sampleSceneIdx]);
    }

    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) !=
        0) {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    LoopData loopdata;

#ifdef __APPLE__
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_FLAGS,
        SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);  // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    SDL_WindowFlags window_flags =
        (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                          SDL_WINDOW_ALLOW_HIGHDPI);
    loopdata.mWindow = SDL_CreateWindow(
        "GLSL PathTracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        renderOptions.windowResolution.x, renderOptions.windowResolution.y,
        window_flags);

    // Query actual drawable window size
    int w, h;
    SDL_GL_GetDrawableSize(loopdata.mWindow, &w, &h);
    renderOptions.windowResolution.x = w;
    renderOptions.windowResolution.y = h;

    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

    loopdata.mGLContext = SDL_GL_CreateContext(loopdata.mWindow);
    if (!loopdata.mGLContext) {
        fprintf(stderr, "Failed to initialize GL context!\n");
        return 1;
    }
    SDL_GL_SetSwapInterval(0);  // Disable vsync

    // Initialize OpenGL loader
#if GL_VERSION_3_2
    bool err = gl3wInit() != 0;

    if (err) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }
#endif

    if (!InitRenderer())
    {
        return 1;
    }

    while (!done)
    {
        MainLoop(&loopdata);
    }

    delete renderer;
    delete scene;

    // Cleanup
    SDL_GL_DeleteContext(loopdata.mGLContext);
    SDL_DestroyWindow(loopdata.mWindow);
    SDL_Quit();
    return 0;
}

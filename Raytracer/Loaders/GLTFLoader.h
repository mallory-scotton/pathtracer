// FIXME: THE WHOLE FILE
#pragma once

#include "Components/Scene.hpp"

namespace Ray {
class Scene;

bool LoadGLTF(const std::string& filename, Scene* scene,
    Renderer::Options& renderOptions, Mat4x4f xform, bool binary);
}  // namespace Ray

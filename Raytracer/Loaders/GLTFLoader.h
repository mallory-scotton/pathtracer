// FIXME: THE WHOLE FILE
#pragma once

#include "Components/Scene.h"

namespace Ray {
class Scene;

bool LoadGLTF(const std::string& filename, Scene* scene,
              RenderOptions& renderOptions, Mat4x4f xform, bool binary);
}  // namespace Ray

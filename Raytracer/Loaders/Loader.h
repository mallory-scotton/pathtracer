// FIXME: THE WHOLE FILE
#pragma once

#include "Components/Scene.h"

namespace Ray {
class Scene;

bool LoadSceneFromFile(const std::string& filename, Scene* scene,
                       RenderOptions& renderOptions);
}  // namespace Ray

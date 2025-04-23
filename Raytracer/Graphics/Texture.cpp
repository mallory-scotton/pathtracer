///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include "Graphics/Texture.hpp"
#include "ImGui/stb_image.h"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
Texture::Texture(void)
    : width(0)
    , height(0)
    , components(0)
{}

///////////////////////////////////////////////////////////////////////////////
Texture::Texture(
    const String& name,
    Byte* data,
    int width,
    int height,
    int components
)
    : name(name)
    , width(width)
    , height(height)
    , components(components)
{
    RAY_INFO("Copying texture: \"" << name << "\"");
    texData.resize(width * height * components);
    std::copy(data, data + width * height * components, texData.begin());
}

///////////////////////////////////////////////////////////////////////////////
Texture::~Texture()
{
    texData.clear();
}

///////////////////////////////////////////////////////////////////////////////
bool Texture::LoadTexture(const Path& filepath)
{
    name = filepath.string();
    components = 4;

    Byte* data = stbi_load(filepath.c_str(), &width, &height, NULL, components);

    if (data == nullptr)
    {
        RAY_WARN("Unable to load " << filepath);
        return (false);
    }

    texData.resize(width * height * components);
    std::copy(data, data + width * height * components, texData.begin());
    stbi_image_free(data);

    return (true);
}

} // namespace Ray

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Resources/Texture.hpp"
#include "Utils/Exception.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <imstb_image.h>

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
Texture::Texture(const Path& filePath)
{
    if (!LoadFromFile(filePath))
    {
        throw Exception(RAY_ERROR_FAILED_TEXTURE);
    }
}

///////////////////////////////////////////////////////////////////////////////
Texture::Texture(
    const String& name,
    unsigned char* data,
    int width,
    int height,
    int components
)
    : mWidth(width)
    , mHeight(height)
    , mComponents(components)
    , mName(name)
{
    mData.resize(width * height * components);
    std::copy(data, data + width * height * components, mData.begin());
}

///////////////////////////////////////////////////////////////////////////////
Texture::Texture(
    const String& name,
    Vector<Byte> data,
    int width,
    int height,
    int components
)
    : mWidth(width)
    , mHeight(height)
    , mComponents(components)
    , mName(name)
{
    mData.resize(width * height * components);
    std::copy(data.begin(), data.end(), mData.begin());
}

///////////////////////////////////////////////////////////////////////////////
bool Texture::LoadFromFile(const Path& filePath)
{
    mName = filePath.string();
    mComponents = 4;

    unsigned char* data = stbi_load(
        filePath.c_str(),
        &mWidth, &mHeight,
        nullptr, mComponents
    );

    if (data == nullptr)
    {
        return (false);
    }

    mData.resize(mWidth * mHeight * mComponents);
    std::copy(data, data + mWidth * mHeight * mComponents, mData.begin());

    stbi_image_free(data);
    return (true);
}

///////////////////////////////////////////////////////////////////////////////
String Texture::GetName(void) const
{
    return (mName);
}

///////////////////////////////////////////////////////////////////////////////
int Texture::GetWidth(void) const
{
    return (mWidth);
}

///////////////////////////////////////////////////////////////////////////////
int Texture::GetHeight(void) const
{
    return (mHeight);
}

///////////////////////////////////////////////////////////////////////////////
const Vector<Byte>& Texture::GetData(void) const
{
    return (mData);
}

} // namespace Ray

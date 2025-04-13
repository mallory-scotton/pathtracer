///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Resources/EnvMap.hpp"
#include "Maths/Utils.hpp"
#include "Utils/Exception.hpp"
#include <imstb_image.h>

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
EnvMap::EnvMap(void)
    : mWidth(0)
    , mHeight(0)
    , mImage(nullptr)
{}

///////////////////////////////////////////////////////////////////////////////
EnvMap::EnvMap(const Path& filePath)
    : EnvMap()
{
    if (!LoadFromFile(filePath))
    {
        throw Exception(
            "Failed to load Environment Map: \"" + filePath.string() + "\""
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
EnvMap::~EnvMap()
{
    stbi_image_free(mImage);
    mCDF.clear();
}

///////////////////////////////////////////////////////////////////////////////
bool EnvMap::LoadFromFile(const Path& filePath)
{
    mImage = stbi_loadf(filePath.c_str(), &mWidth, &mHeight, nullptr, 3);

    if (mImage == nullptr)
    {
        return (false);
    }

    BuildCDF();

    return (true);
}

///////////////////////////////////////////////////////////////////////////////
void EnvMap::BuildCDF(void)
{
    Vector<float> weights;

    weights.resize(mWidth * mHeight);

    for (int v = 0; v < mHeight; v++)
    {
        for (int u = 0; u < mWidth; u++)
        {
            int idx = v * mWidth * 3 + u * 3;
            weights[u + v * mWidth] = Math::Luminance(
                mImage[idx + 0], mImage[idx + 1], mImage[idx + 2]
            );
        }
    }

    mCDF.resize(mWidth * mHeight);

    mCDF[0] = weights[0];
    for (int i = 1; i < mWidth * mHeight; i++)
    {
        mCDF[i] = mCDF[i - 1] + weights[i];
    }

    mTotalSum = mCDF[mWidth * mHeight - 1];
    weights.clear();
}

} // namespace Ray

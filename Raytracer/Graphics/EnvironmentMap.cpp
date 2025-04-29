///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Graphics/EnvironmentMap.hpp"
#include <math.h>
#include <memory.h>
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
struct StbiImageDeleter
{
    void operator()(float *ptr) const
    {
        if (ptr)
        {
            stbi_image_free(ptr);
        }
    }
};

///////////////////////////////////////////////////////////////////////////////
EnvironmentMap::EnvironmentMap(void)
    : width(0)
    , height(0)
    , totalSum(0.f)
    , img(nullptr)
    , cdf(nullptr)
{}

///////////////////////////////////////////////////////////////////////////////
EnvironmentMap::~EnvironmentMap()
{}

///////////////////////////////////////////////////////////////////////////////
void EnvironmentMap::BuildCDF(void)
{
    Vector<float> weights(static_cast<size_t>(width) * height);
    float* imgData = img.get();

    for (int v = 0; v < height; v++)
    {
        for (int u = 0; u < width; u++)
        {
            if (!imgData)
            {
                continue;
            }

            int imgIdx = (v * width + u) * 3;
            weights[
                static_cast<size_t>(u) + static_cast<size_t>(v) * width
            ] = Math::Luminance(
                imgData[imgIdx + 0], imgData[imgIdx + 1], imgData[imgIdx + 2]
            );
        }
    }

    cdf = std::make_unique<float[]>(static_cast<size_t>(width) * height);
    if (weights.empty()) {
        totalSum = 0.0f;
        return;
    }

    cdf[0] = weights[0];
    for (size_t i = 1; i < static_cast<size_t>(width) * height; i++)
    {
        cdf[i] = cdf[i - 1] + weights[i];
    }

    totalSum = cdf[static_cast<size_t>(width) * height - 1];
}

///////////////////////////////////////////////////////////////////////////////
bool EnvironmentMap::LoadMap(const String& filename)
{
    int w, h, channels;
    float* raw_img = stbi_loadf(filename.c_str(), &w, &h, &channels, 3);

    if (raw_img == nullptr)
    {
        RAY_WARN(
            RAY_ERROR_FAILED_TEXTURE << ": " <<
            filename << " - " << stbi_failure_reason()
        );
        width = 0;
        height = 0;
        img.reset();
        cdf.reset();
        totalSum = 0.0f;
        return (false);
    }

    img.reset(raw_img);
    width = w;
    height = h;

    if (width <= 0 || height <= 0)
    {
        RAY_WARN("Invalid image dimensions loaded for: " << filename);
        img.reset();
        width = 0;
        height = 0;
        cdf.reset();
        totalSum = 0.0f;
        return (false);
    }

    BuildCDF();

    return (true);
}

} // namespace Ray

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Scene/Material.hpp"
#include "Renderer/Scene.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
Material::Material(const Map<String, Vector<String>>& props, Scene* scene)
{
    for (const auto& [key, values] : props)
    {
        Uint64 n = values.size();

        if (key == "color" && n == 3)
        {
            mBaseColor = Color(
                std::stof(values[0]),
                std::stof(values[1]),
                std::stof(values[2])
            );
        }
        else if (key == "opacity" && n == 1)
        {
            mOpacity = std::stof(values[0]);
        }
        else if (key == "alphaMode" && n == 1)
        {
            if (values[0] == "opaque")
            {
                mAlphaMode = static_cast<float>(AlphaMode::OPAQUE);
            }
            else if (values[0] == "blend")
            {
                mAlphaMode = static_cast<float>(AlphaMode::BLEND);
            }
            else if (values[0] == "mask")
            {
                mAlphaMode = static_cast<float>(AlphaMode::MASK);
            }
        }
        else if (key == "alphaCutOff" && n == 1)
        {
            mAlphaCutOff = std::stof(values[0]);
        }
        else if (key == "emission" && n == 3)
        {
            mEmission = Color(
                std::stof(values[0]),
                std::stof(values[1]),
                std::stof(values[2])
            );
        }
        else if (key == "metallic" && n == 1)
        {
            mMetallic = std::stof(values[0]);
        }
        else if (key == "roughness" && n == 1)
        {
            mRoughness = std::stof(values[0]);
        }
        else if (key == "subSurface" && n == 1)
        {
            mSubSurface = std::stof(values[0]);
        }
        else if (key == "specularTint" && n == 1)
        {
            mSpecularTint = std::stof(values[0]);
        }
        else if (key == "anisotropic" && n == 1)
        {
            mAnisotropic = std::stof(values[0]);
        }
        else if (key == "sheen" && n == 1)
        {
            mSheen = std::stof(values[0]);
        }
        else if (key == "sheenTint" && n == 1)
        {
            mSheenTint = std::stof(values[0]);
        }
        else if (key == "clearCoat" && n == 1)
        {
            mClearCoat = std::stof(values[0]);
        }
        else if (key == "clearCoatGloss" && n == 1)
        {
            mClearCoatGloss = std::stof(values[0]);
        }
        else if (key == "specTrans" && n == 1)
        {
            mSpecTrans = std::stof(values[0]);
        }
        else if (key == "ior" && n == 1)
        {
            mIor = std::stof(values[0]);
        }
        else if (key == "albedoTexture" && n == 1)
        {
            mbaseColorTextureID = static_cast<float>(
                scene->AddTexture(values[0])
            );
        }
        else if (key == "metallicRoughnessTexture" && n == 1)
        {
            mMetallicRoughnessTextureID = static_cast<float>(
                scene->AddTexture(values[0])
            );
        }
        else if (key == "normalTexture" && n == 1)
        {
            mNormalMapTextureID = static_cast<float>(
                scene->AddTexture(values[0])
            );
        }
        else if (key == "emissionTexture" && n == 1)
        {
            mEmissionMapTextureID = static_cast<float>(
                scene->AddTexture(values[0])
            );
        }
        else if (key == "mediumType" && n == 1)
        {
            if (values[0] == "absorb")
            {
                mMediumType = static_cast<float>(MediumType::ABSORB);
            }
            else if (values[0] == "scatter")
            {
                mMediumType = static_cast<float>(MediumType::SCATTER);
            }
            else if (values[0] == "emissive")
            {
                mMediumType = static_cast<float>(MediumType::EMISSIVE);
            }
        }
        else if (key == "mediumDensity" && n == 1)
        {
            mMediumDensity = std::stof(values[0]);
        }
        else if (key == "mediumColor" && n == 3)
        {
            mMediumColor = Color(
                std::stof(values[0]),
                std::stof(values[1]),
                std::stof(values[2])
            );
        }
        else if (key == "mediumAnisotropy" && n == 1)
        {
            mMediumAnisotropy = std::stof(values[0]);
        }
        else
        {
            RAY_WARN("\"" + key + "\" is not a valid Material attributes");
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
Material::AlphaMode Material::GetAlphaMode(void) const
{
    return (static_cast<AlphaMode>(mAlphaMode));
}

///////////////////////////////////////////////////////////////////////////////
Material::MediumType Material::GetMediumType(void) const
{
    return (static_cast<MediumType>(mMediumType));
}

} // namespace Ray

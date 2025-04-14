///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Scene/Material.hpp"
#include "Renderer/Scene.hpp"
#include "Utils/Utils.hpp"

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

        if (Utils::Equals(key, "color") && n == 3)
        {
            mBaseColor = Color(
                Utils::ToFloat(values[0]),
                Utils::ToFloat(values[1]),
                Utils::ToFloat(values[2])
            );
        }
        else if (Utils::Equals(key, "opacity") && n == 1)
        {
            mOpacity = Utils::ToFloat(values[0]);
        }
        else if (Utils::Equals(key, "alphaMode") && n == 1)
        {
            if (Utils::Equals(values[0], "opaque"))
            {
                mAlphaMode = static_cast<float>(AlphaMode::OPAQUE);
            }
            else if (Utils::Equals(values[0], "blend"))
            {
                mAlphaMode = static_cast<float>(AlphaMode::BLEND);
            }
            else if (Utils::Equals(values[0], "mask"))
            {
                mAlphaMode = static_cast<float>(AlphaMode::MASK);
            }
        }
        else if (Utils::Equals(key, "alphaCutOff") && n == 1)
        {
            mAlphaCutOff = Utils::ToFloat(values[0]);
        }
        else if (Utils::Equals(key, "emission") && n == 3)
        {
            mEmission = Color(
                Utils::ToFloat(values[0]),
                Utils::ToFloat(values[1]),
                Utils::ToFloat(values[2])
            );
        }
        else if (Utils::Equals(key, "metallic") && n == 1)
        {
            mMetallic = Utils::ToFloat(values[0]);
        }
        else if (Utils::Equals(key, "roughness") && n == 1)
        {
            mRoughness = Utils::ToFloat(values[0]);
        }
        else if (Utils::Equals(key, "subSurface") && n == 1)
        {
            mSubSurface = Utils::ToFloat(values[0]);
        }
        else if (Utils::Equals(key, "specularTint") && n == 1)
        {
            mSpecularTint = Utils::ToFloat(values[0]);
        }
        else if (Utils::Equals(key, "anisotropic") && n == 1)
        {
            mAnisotropic = Utils::ToFloat(values[0]);
        }
        else if (Utils::Equals(key, "sheen") && n == 1)
        {
            mSheen = Utils::ToFloat(values[0]);
        }
        else if (Utils::Equals(key, "sheenTint") && n == 1)
        {
            mSheenTint = Utils::ToFloat(values[0]);
        }
        else if (Utils::Equals(key, "clearCoat") && n == 1)
        {
            mClearCoat = Utils::ToFloat(values[0]);
        }
        else if (Utils::Equals(key, "clearCoatGloss") && n == 1)
        {
            mClearCoatGloss = Utils::ToFloat(values[0]);
        }
        else if (Utils::Equals(key, "specTrans") && n == 1)
        {
            mSpecTrans = Utils::ToFloat(values[0]);
        }
        else if (Utils::Equals(key, "ior") && n == 1)
        {
            mIor = Utils::ToFloat(values[0]);
        }
        else if (Utils::Equals(key, "albedoTexture") && n == 1)
        {
            mbaseColorTextureID = static_cast<float>(
                scene->AddTexture(values[0])
            );
        }
        else if (Utils::Equals(key, "metallicRoughnessTexture") && n == 1)
        {
            mMetallicRoughnessTextureID = static_cast<float>(
                scene->AddTexture(values[0])
            );
        }
        else if (Utils::Equals(key, "normalTexture") && n == 1)
        {
            mNormalMapTextureID = static_cast<float>(
                scene->AddTexture(values[0])
            );
        }
        else if (Utils::Equals(key, "emissionTexture") && n == 1)
        {
            mEmissionMapTextureID = static_cast<float>(
                scene->AddTexture(values[0])
            );
        }
        else if (Utils::Equals(key, "mediumType") && n == 1)
        {
            if (Utils::Equals(values[0], "absorb"))
            {
                mMediumType = static_cast<float>(MediumType::ABSORB);
            }
            else if (Utils::Equals(values[0], "scatter"))
            {
                mMediumType = static_cast<float>(MediumType::SCATTER);
            }
            else if (Utils::Equals(values[0], "emissive"))
            {
                mMediumType = static_cast<float>(MediumType::EMISSIVE);
            }
        }
        else if (Utils::Equals(key, "mediumDensity") && n == 1)
        {
            mMediumDensity = Utils::ToFloat(values[0]);
        }
        else if (Utils::Equals(key, "mediumColor") && n == 3)
        {
            mMediumColor = Color(
                Utils::ToFloat(values[0]),
                Utils::ToFloat(values[1]),
                Utils::ToFloat(values[2])
            );
        }
        else if (Utils::Equals(key, "mediumAnisotropy") && n == 1)
        {
            mMediumAnisotropy = Utils::ToFloat(values[0]);
        } else if (Utils::Equals(key, "name") && n == 1)
        {
            // Do nothing, already handled in Scene
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

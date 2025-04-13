///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Scene/Light.hpp"
#include "Maths/Constants.hpp"
#include "Utils/Utils.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
Light::Light(const Map<String, Vector<String>>& props)
{
    Vec3f v1, v2;

    for (const auto& [key, values] : props)
    {
        Uint64 n = values.size();

        if (Utils::Equals(key, "position") && n == 3)
        {
            mPosition = Vec3f(
                Utils::ToFloat(values[0]),
                Utils::ToFloat(values[1]),
                Utils::ToFloat(values[2])
            );
        }
        else if (Utils::Equals(key, "emission") && n == 3)
        {
            mEmission = Vec3f(
                Utils::ToFloat(values[0]),
                Utils::ToFloat(values[1]),
                Utils::ToFloat(values[2])
            );
        }
        else if (Utils::Equals(key, "radius") && n == 1)
        {
            mRadius = Utils::ToFloat(values[0]);
        }
        else if (Utils::Equals(key, "v1") && n == 3)
        {
            v1 = Vec3f(
                Utils::ToFloat(values[0]),
                Utils::ToFloat(values[1]),
                Utils::ToFloat(values[2])
            );
        }
        else if (Utils::Equals(key, "v2") && n == 3)
        {
            v2 = Vec3f(
                Utils::ToFloat(values[0]),
                Utils::ToFloat(values[1]),
                Utils::ToFloat(values[2])
            );
        }
        else if (Utils::Equals(key, "type") && n == 1)
        {
            if (values[0] == "quad")
            {
                mType = Light::Type::RECT;
            }
            else if (values[0] == "sphere")
            {
                mType = Light::Type::SPHERE;
            }
            else if (values[0] == "distant")
            {
                mType = Light::Type::DISTANT;
            }
        }
        else
        {
            RAY_WARN("\"" + key + "\" is not a valid Light attributes");
        }
    }

    switch (mType)
    {
        case Light::Type::RECT:
            mU = v1 - mPosition;
            mV = v2 - mPosition;
            mArea = Vec3f::Length(Vec3f::Cross(mU, mV));
            break;
        case Light::Type::SPHERE:
            mArea = 4.f * PI * mRadius * mRadius;
            break;
        case Light::Type::DISTANT:
            mArea = 0.f;
            break;
    }
}

} // namespace Ray

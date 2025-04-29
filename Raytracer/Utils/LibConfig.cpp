///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Utils/LibConfig.hpp"
#include "Errors/ParsingException.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
LibConfig::LibConfig(const Path& filePath)
{
    try
    {
        m_config.readFile(filePath.string());
    }
    catch (const libconfig::FileIOException& error)
    {
        String message = "Opening configuration file: ";

        message += error.what();

        throw ParsingException(message);
    }
    catch (const libconfig::ParseException& error)
    {
        String message = "In configuration file at ";

        message += error.getFile();
        message += ":";
        message += error.getLine();
        message += " - ";
        message += error.getError();

        throw ParsingException(message);
    }
}

///////////////////////////////////////////////////////////////////////////////
bool LibConfig::Exists(const String& path)
{
    return (m_config.exists(path));
}

///////////////////////////////////////////////////////////////////////////////
Optional<LibConfig::Setting> LibConfig::Lookup(const String& path) const
{
    if (m_config.exists(path))
    {
        return (std::make_optional<LibConfig::Setting>(m_config.lookup(path)));
    }

    return (std::nullopt);
}

///////////////////////////////////////////////////////////////////////////////
bool LibConfig::Value(const String& path, float& data) const
{
    return (m_config.lookupValue(path, data));
}

///////////////////////////////////////////////////////////////////////////////
bool LibConfig::Value(const String& path, int& data) const
{
    return (m_config.lookupValue(path, data));
}

///////////////////////////////////////////////////////////////////////////////
bool LibConfig::Value(const String& path, String& data) const
{
    return (m_config.lookupValue(path, data));
}

///////////////////////////////////////////////////////////////////////////////
bool LibConfig::Value(const String& path, Vec2i& data) const
{
    if (m_config.exists(path))
    {
        const Setting& vector = m_config.lookup(path);

        if (!vector.Exists("x") || !vector.Exists("y"))
        {
            return (false);
        }

        vector.Value("x", data.x);
        vector.Value("y", data.y);

        return (true);
    }
    return (false);
}

///////////////////////////////////////////////////////////////////////////////
bool LibConfig::Value(const String& path, Vec2f& data) const
{
    if (m_config.exists(path))
    {
        const Setting& vector = m_config.lookup(path);

        if (!vector.Exists("x") || !vector.Exists("y"))
        {
            return (false);
        }

        vector.Value("x", data.x);
        vector.Value("y", data.y);

        return (true);
    }
    return (false);
}

///////////////////////////////////////////////////////////////////////////////
bool LibConfig::Value(const String& path, Vec3i& data) const
{
    if (m_config.exists(path))
    {
        const Setting& vector = m_config.lookup(path);

        if (!vector.Exists("x") || !vector.Exists("y") || !vector.Exists("z"))
        {
            return (false);
        }

        vector.Value("x", data.x);
        vector.Value("y", data.y);
        vector.Value("z", data.z);

        return (true);
    }
    return (false);
}

///////////////////////////////////////////////////////////////////////////////
bool LibConfig::Value(const String& path, Vec3f& data) const
{
    if (m_config.exists(path))
    {
        const Setting& vector = m_config.lookup(path);

        if (!vector.Exists("x") || !vector.Exists("y") || !vector.Exists("z"))
        {
            return (false);
        }

        vector.Value("x", data.x);
        vector.Value("y", data.y);
        vector.Value("z", data.z);

        return (true);
    }
    return (false);
}

///////////////////////////////////////////////////////////////////////////////
LibConfig::Setting::Setting(const libconfig::Setting& native)
    : m_native(native)
{}

///////////////////////////////////////////////////////////////////////////////
bool LibConfig::Setting::Exists(const String& path) const
{
    return (m_native.exists(path));
}

///////////////////////////////////////////////////////////////////////////////
Optional<LibConfig::Setting> LibConfig::Setting::Lookup(
    const String& path
) const
{
    if (m_native.exists(path))
    {
        return (std::make_optional<LibConfig::Setting>(m_native.lookup(path)));
    }

    return (std::nullopt);
}

///////////////////////////////////////////////////////////////////////////////
bool LibConfig::Setting::Value(const String& path, float& data) const
{
    return (m_native.lookupValue(path, data));
}

///////////////////////////////////////////////////////////////////////////////
bool LibConfig::Setting::Value(const String& path, int& data) const
{
    return (m_native.lookupValue(path, data));
}

///////////////////////////////////////////////////////////////////////////////
bool LibConfig::Setting::Value(const String& path, String& data) const
{
    return (m_native.lookupValue(path, data));
}

///////////////////////////////////////////////////////////////////////////////
bool LibConfig::Setting::Value(const String& path, Vec2i& data) const
{
    if (m_native.exists(path))
    {
        const Setting& vector = m_native.lookup(path);

        if (!vector.Exists("x") || !vector.Exists("y"))
        {
            return (false);
        }

        vector.Value("x", data.x);
        vector.Value("y", data.y);

        return (true);
    }
    return (false);
}

///////////////////////////////////////////////////////////////////////////////
bool LibConfig::Setting::Value(const String& path, Vec2f& data) const
{
    if (m_native.exists(path))
    {
        const Setting& vector = m_native.lookup(path);

        if (!vector.Exists("x") || !vector.Exists("y"))
        {
            return (false);
        }

        vector.Value("x", data.x);
        vector.Value("y", data.y);

        return (true);
    }
    return (false);
}

///////////////////////////////////////////////////////////////////////////////
bool LibConfig::Setting::Value(const String& path, Vec3i& data) const
{
    if (m_native.exists(path))
    {
        const Setting& vector = m_native.lookup(path);

        if (!vector.Exists("x") || !vector.Exists("y") || !vector.Exists("z"))
        {
            return (false);
        }

        vector.Value("x", data.x);
        vector.Value("y", data.y);
        vector.Value("z", data.z);

        return (true);
    }
    return (false);
}

///////////////////////////////////////////////////////////////////////////////
bool LibConfig::Setting::Value(const String& path, Vec3f& data) const
{
    if (m_native.exists(path))
    {
        const Setting& vector = m_native.lookup(path);

        if (!vector.Exists("x") || !vector.Exists("y") || !vector.Exists("z"))
        {
            return (false);
        }

        vector.Value("x", data.x);
        vector.Value("y", data.y);
        vector.Value("z", data.z);

        return (true);
    }
    return (false);
}

///////////////////////////////////////////////////////////////////////////////
int LibConfig::Setting::Length(void) const
{
    return (m_native.getLength());
}

///////////////////////////////////////////////////////////////////////////////
const LibConfig::Setting& LibConfig::Setting::At(int index) const
{
    return (m_native[index]);
}

} // namespace Ray

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Utils/FileSystem.hpp"
#include <filesystem>

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
namespace sfs = std::filesystem;

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
bool Fs::Mkdir(const Path& path)
{
    std::error_code errorCode;
    return (sfs::create_directories(path, errorCode) && !errorCode);
}

///////////////////////////////////////////////////////////////////////////////
bool Fs::Exists(const Path& path)
{
    std::error_code errorCode;
    return (sfs::exists(path, errorCode) && !errorCode);
}

///////////////////////////////////////////////////////////////////////////////
Uint64 Fs::Size(const Path& path)
{
    std::error_code errorCode;

    if (!Exists(path) || IsDirectory(path))
    {
        return (0);
    }

    return (static_cast<Uint64>(sfs::file_size(path, errorCode)));
}

///////////////////////////////////////////////////////////////////////////////
bool Fs::Delete(const Path& path)
{
    std::error_code errorCode;
    return (sfs::remove_all(path, errorCode) && !errorCode);
}

///////////////////////////////////////////////////////////////////////////////
bool Fs::Rename(const Path& oldPath, const Path& newPath)
{
    std::error_code errorCode;
    sfs::rename(oldPath, newPath, errorCode);
    return (!errorCode);
}

///////////////////////////////////////////////////////////////////////////////
bool Fs::Copy(const Path& source, const Path& destination)
{
    std::error_code errorCode;
    auto options =
        sfs::copy_options::overwrite_existing |
        sfs::copy_options::recursive;
    sfs::copy(source, destination, options, errorCode);
    return (!errorCode);
}

///////////////////////////////////////////////////////////////////////////////
Vector<Path> Fs::ListDirectory(const Path& path)
{
    Vector<Path> entries;
    std::error_code errorCode;

    if (!IsDirectory(path))
    {
        return (entries);
    }

    for (const auto& entry : sfs::directory_iterator(path, errorCode))
    {
        if (errorCode)
        {
            break;
        }
        entries.push_back(entry.path());
    }
    return (entries);
}

///////////////////////////////////////////////////////////////////////////////
Vector<Path> Fs::DiscoverFilesByExtensions(
    const Path& path,
    const Vector<String>& extensions,
    bool recursive
)
{
    Vector<Path> files;
    std::error_code errorCode;

    if (!Exists(path))
    {
        return (files);
    }

    auto checkExtension = [&](const Path& filePath) {
        if (!IsFile(filePath))
        {
            return;
        }
        String ext = GetExtension(filePath);
        for (const auto& allowedExt : extensions)
        {
            String cleanAllowedExt = allowedExt;

            if (!cleanAllowedExt.empty() && cleanAllowedExt[0] != '.')
            {
                cleanAllowedExt.insert(0, ".");
            }

            if (ext == cleanAllowedExt)
            {
                files.push_back(filePath);
                break;
            }
        }
    };

    if (recursive)
    {
        for (const auto& entry :
            sfs::recursive_directory_iterator(path, errorCode))
        {
            if (errorCode) break;
            checkExtension(entry.path());
        }
    }
    else
    {
        for (const auto& entry : sfs::directory_iterator(path, errorCode))
        {
            if (errorCode) break;
            checkExtension(entry.path());
        }
    }

    return (files);
}

///////////////////////////////////////////////////////////////////////////////
String Fs::GetExtension(const Path& path)
{
    return (path.extension().string());
}

///////////////////////////////////////////////////////////////////////////////
bool Fs::IsDirectory(const Path& path)
{
    std::error_code errorCode;
    bool result = sfs::is_directory(path, errorCode);
    return (result && !errorCode);
}

///////////////////////////////////////////////////////////////////////////////
bool Fs::IsFile(const Path& path)
{
    std::error_code errorCode;
    bool result = sfs::is_regular_file(path, errorCode);
    return (result && !errorCode);
}

} // namespace Ray

///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Config.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class Texture
{
private:
    ///////////////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////////////
    int mWidth{0};          //<!
    int mHeight{0};         //<!
    int mComponents{0};     //<!
    Vector<Byte> mData;     //<!
    String mName;           //<!

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    Texture(void) = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param filePath
    ///
    ///////////////////////////////////////////////////////////////////////////
    Texture(const Path& filePath);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param name
    /// \param data
    /// \param width
    /// \param height
    /// \param components
    ///
    ///////////////////////////////////////////////////////////////////////////
    Texture(
        const String& name,
        unsigned char* data,
        int width,
        int height,
        int components
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param name
    /// \param data
    /// \param width
    /// \param height
    /// \param components
    ///
    ///////////////////////////////////////////////////////////////////////////
    Texture(
        const String& name,
        Vector<Byte> data,
        int width,
        int height,
        int components
    );

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param filePath
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool LoadFromFile(const Path& filePath);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    String GetName(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    int GetWidth(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    int GetHeight(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    const Vector<Byte>& GetData(void) const;
};

} // namespace Ray

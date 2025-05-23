///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Config.hpp"
#include <vector>
#include <algorithm>

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
public:
    ///////////////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////////////
    String name;                    //<!
    int width;                      //<!
    int height;                     //<!
    int components;                 //<!
    Vector<Byte> texData;           //<!

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    Texture(void);

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
        Byte* data,
        int width,
        int height,
        int components
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    ~Texture();

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param filepath
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool LoadTexture(const Path& filepath);
};
} // namespace Ray

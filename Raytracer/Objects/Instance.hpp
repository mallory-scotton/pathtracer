///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Config.hpp"
#include "Maths/Mat4x4.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class Instance
{
public:
    ///////////////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////////////
    String name;                //<!
    int objectID;               //<!
    Mat4x4f transform;          //<!
    int materialID;             //<!

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param name
    /// \param objectID
    /// \param transform
    /// \param materialID
    ///
    ///////////////////////////////////////////////////////////////////////////
    Instance(
        const String& name = "default",
        int objectID = -1,
        const Mat4x4f& transform = Mat4x4f(),
        int materialID = 0
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual ~Instance() = default;
};

} // namespace Ray

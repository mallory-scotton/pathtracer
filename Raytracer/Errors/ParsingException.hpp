///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Errors/Exception.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class ParsingException : public Exception
{
public: 
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param message
    ///
    ///////////////////////////////////////////////////////////////////////////
    ParsingException(const String& message);
};

} // namespace Ray

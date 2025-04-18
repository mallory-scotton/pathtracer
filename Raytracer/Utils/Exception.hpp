///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Config.hpp"
#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class Exception : public std::exception
{
private:
    ///////////////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////////////
    String mMesssage;

public: 
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param message
    ///
    ///////////////////////////////////////////////////////////////////////////
    Exception(const String& message);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual const char* what(void) const noexcept override;
};

} // namespace Ray

///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Config.hpp"
#include "Objects.hpp"
#include "Utils/Singleton.hpp"
#include "Utils/LibConfig.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class IFactory
{
public:
    ///////////////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////////////
    using Constructor = Function<
        UniquePtr<T>(const Optional<LibConfig::Setting>&)
    >;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    IFactory(void) = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual ~IFactory() = default;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param name
    /// \param constructor
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual IFactory& Register(
        const String& name,
        const typename IFactory<T>::Constructor& constructor
    ) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param name
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool HasConstructor(const String& name) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param name
    /// \param config
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual Optional<UniquePtr<T>> Create(
        const String& name,
        const Optional<LibConfig::Setting>& config = std::nullopt
    ) const = 0;
};

} // namespace Ray

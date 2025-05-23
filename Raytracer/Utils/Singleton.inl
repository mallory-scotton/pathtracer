///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Utils/Singleton.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
template <typename T>
UniquePtr<T> Singleton<T>::m_instance;

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Mutex Singleton<T>::m_mutex;

///////////////////////////////////////////////////////////////////////////////
template <typename T>
T& Singleton<T>::GetInstance(void)
{
    std::lock_guard<Mutex> lock(m_mutex);

    if (!m_instance)
    {
        m_instance = std::make_unique<T>();
    }
    return (*m_instance);
}

} // namespace Ray

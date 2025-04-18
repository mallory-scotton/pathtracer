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
class EnvMap
{
private:
    ///////////////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////////////
    int mWidth;             //<!
    int mHeight;            //<!
    float mTotalSum;        //<!
    float* mImage;          //<!
    Vector<float> mCDF;     //<!

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    EnvMap(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param filePath
    ///
    ///////////////////////////////////////////////////////////////////////////
    EnvMap(const Path& filePath);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    ~EnvMap();

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void BuildCDF(void);

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
};

} // namespace Ray

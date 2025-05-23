///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Config.hpp"
#include "Graphics/EnvironmentMap.hpp"
#include "Acceleration/bvh.h"
#include "Core/Renderer.hpp"
#include "Objects/Instance.hpp"
#include "Interfaces/IObject.hpp"
#include "Components/Camera.hpp"
#include "Acceleration/bvh_translator.h"
#include "Graphics/Texture.hpp"
#include "Components/Material.hpp"
#include "Components/Light.hpp"
#include "Maths/Index.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class Scene
{
public:
    ///////////////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////////////
    Renderer::Options renderOptions;            //<!
    Vector<UniquePtr<IObject>> objects;         //<!
    Vector<Index> vertIndices;                  //<!
    Vector<Vec4f> verticesUVX;                  //<!
    Vector<Vec4f> normalsUVY;                   //<!
    Vector<Mat4x4f> transforms;                 //<!
    Vector<Material> materials;                 //<!
    Vector<String> materialsName;               //<!
    Vector<Instance> instances;                 //<!
    Vector<Light> lights;                       //<!
    UniquePtr<EnvironmentMap> envMap;           //<!
    UniquePtr<Camera> camera;                   //<!
    BvhTranslator bvhTranslator;                //<!
    BoundingBox sceneBounds;                    //<!
    Vector<UniquePtr<Texture>> textures;        //<!
    Vector<unsigned char> textureMapsArray;     //<!
    bool initialized;                           //<!
    bool dirty;                                 //<!
    bool instancesModified;                     //<!
    bool envMapModified;                        //<!

private:
    ///////////////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////////////
    UniquePtr<Ray::Bvh> sceneBvh;               //<!

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    Scene(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual ~Scene() = default;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param filename
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    int AddMesh(const Path& filename);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param filename
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    int AddTexture(const Path& filename);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param material
    /// \param name
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    int AddMaterial(const Material& material, String name);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param instance
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    int AddMeshInstance(const Instance& meshInstance);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param light
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    int AddLight(const Light& light);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param name
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    int getMaterialID(const String& name);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param filename
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    void AddEnvMap(const Path& filename);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param sampleCounter
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool CanUpdate(int sampleCounter) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void ProcessScene(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void RebuildInstances(void);

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void createBLAS(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void createTLAS(void);
};

} // namespace Ray

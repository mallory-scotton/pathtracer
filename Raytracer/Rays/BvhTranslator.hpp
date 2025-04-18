///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Config.hpp"
#include "Resources/Mesh.hpp"
#include "Rays/Bvh.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class BvhTranslator
{
public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct Node
    {
        Vec3f min;
        Vec3f max;
        Vec3f leaf;
    };

private:
    ///////////////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////////////
    int mTopLevelIndex{0};                  //<!
    Vector<Node> mNodes;                    //<!
    int mNodeTextureWidth;                  //<!
    int mCurrentNode{0};                    //<!
    int mCurrentTriIndex{0};                //<!
    Vector<int> mBvhRootStartIndices;       //<!
    Vector<Mesh::Instance> mMeshInstances;  //<!
    Vector<SharedPtr<Mesh>> mMeshes;        //<!
    const Bvh* mTopLevelBvh;                //<!

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    BvhTranslator(void) = default;

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param root
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    int ProcessBLASNodes(const Bvh::Node* root);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param root
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    int ProcessTLASNodes(const Bvh::Node* root);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void ProcessBLAS(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void ProcessTLAS(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param topLevelBvh
    /// \param instances
    ///
    ///////////////////////////////////////////////////////////////////////////
    void UpdateTLAS(
        const Bvh* topLevelBvh,
        const Vector<Mesh::Instance>& instances
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param topLevelBvh
    /// \param meshes
    /// \param instances
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Process(
        const Bvh* topLevelBvh,
        const Vector<SharedPtr<Mesh>>& meshes,
        const Vector<Mesh::Instance>& instances
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    Uint64 GetNodeCount(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    const Vector<Node>& GetNodes(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    int GetTopLevelIndex(void) const;
};

} // namespace Ray

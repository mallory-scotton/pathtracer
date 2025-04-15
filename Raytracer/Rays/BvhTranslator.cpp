///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Rays/BvhTranslator.hpp"
#include "Rays/BBox.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
int BvhTranslator::ProcessBLASNodes(const Bvh::Node* root)
{
    BBox box = root->bounds;
    int index = mCurrentNode;

    mNodes[index].min = box.GetMin();
    mNodes[index].max = box.GetMax();
    mNodes[index].leaf.z = 0;

    if (root->type == Bvh::Node::Type::LEAF)
    {
        mNodes[index].leaf.x = mCurrentTriIndex + root->startIndex;
        mNodes[index].leaf.y = root->numPrims;
        mNodes[index].leaf.z = 1;
    }
    else
    {
        mCurrentNode++;
        mNodes[index].leaf.x = ProcessBLASNodes(root->left);
        mCurrentNode++;
        mNodes[index].leaf.y = ProcessBLASNodes(root->right);
    }

    return (index);
}

///////////////////////////////////////////////////////////////////////////////
int BvhTranslator::ProcessTLASNodes(const Bvh::Node* root)
{
    BBox box = root->bounds;
    int index = mCurrentNode;

    mNodes[index].min = box.GetMin();
    mNodes[index].max = box.GetMax();
    mNodes[index].leaf.z = 0;

    if (root->type == Bvh::Node::Type::LEAF)
    {
        int instanceIndex = mTopLevelBvh->GetIndices()[root->startIndex];
        int meshIndex = mMeshInstances[instanceIndex].meshID;
        int materialIndex = mMeshInstances[instanceIndex].materialID;

        mNodes[index].leaf.x = mBvhRootStartIndices[meshIndex];
        mNodes[index].leaf.y = materialIndex;
        mNodes[index].leaf.z = -instanceIndex - 1;
    }
    else
    {
        mCurrentNode++;
        mNodes[index].leaf.x = ProcessTLASNodes(root->left);
        mCurrentNode++;
        mNodes[index].leaf.y = ProcessTLASNodes(root->right);
    }

    return (index);
}

///////////////////////////////////////////////////////////////////////////////
void BvhTranslator::ProcessBLAS(void)
{
    int nodeCount = 0;

    for (Uint64 i = 0; i < mMeshes.size(); i++)
    {
        nodeCount += mMeshes[i]->GetBVH()->GetNodeCount();
    }
    mTopLevelIndex = nodeCount;

    nodeCount += 2 * static_cast<int>(mMeshInstances.size());
    mNodes.resize(nodeCount);

    int bvhRootIndex = 0;
    mCurrentTriIndex = 0;

    for (Uint64 i = 0; i < mMeshes.size(); i++)
    {
        auto mesh = mMeshes[i];
        mCurrentNode = bvhRootIndex;

        mBvhRootStartIndices.push_back(bvhRootIndex);
        bvhRootIndex += mesh->GetBVH()->GetNodeCount();

        ProcessBLASNodes(mesh->GetBVH()->GetRoot());
        mCurrentTriIndex += mesh->GetBVH()->GetIndicesCount();
    }
}

///////////////////////////////////////////////////////////////////////////////
void BvhTranslator::ProcessTLAS(void)
{
    mCurrentNode = mTopLevelIndex;
    ProcessTLASNodes(mTopLevelBvh->GetRoot());
}

///////////////////////////////////////////////////////////////////////////////
void BvhTranslator::UpdateTLAS(
    const Bvh* topLevelBvh,
    const Vector<Mesh::Instance>& instances
)
{
    mTopLevelBvh = topLevelBvh;
    mMeshInstances = instances;
    mCurrentNode = mTopLevelIndex;
    ProcessTLASNodes(topLevelBvh->GetRoot());
}

///////////////////////////////////////////////////////////////////////////////
void BvhTranslator::Process(
    const Bvh* topLevelBvh,
    const Vector<SharedPtr<Mesh>>& meshes,
    const Vector<Mesh::Instance>& instances
)
{
    mTopLevelBvh = topLevelBvh;
    mMeshes = meshes;
    mMeshInstances = instances;
    ProcessBLAS();
    ProcessTLAS();
}

} // namespace Ray

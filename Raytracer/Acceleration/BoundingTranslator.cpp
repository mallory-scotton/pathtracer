///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Acceleration/BoundingTranslator.hpp"
#include <cassert>
#include <iostream>
#include <stack>

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
BoundingTranslator::BoundingTranslator()
    : topLevelIndex(0)
    , curNode(0)
    , curTriIndex(0)
{}

///////////////////////////////////////////////////////////////////////////////
BoundingTranslator::~BoundingTranslator()
{}

///////////////////////////////////////////////////////////////////////////////
void BoundingTranslator::ProcessBLAS(std::vector<UniquePtr<IObject>>& meshes)
{
    int nodeCnt = 0;

    for (size_t i = 0; i < meshes.size(); i++)
        nodeCnt += meshes[i]->GetBVH()->m_nodecnt;
    topLevelIndex = nodeCnt;

    // reserve space for top level nodes
    nodeCnt += 2 * meshInstances.size();
    nodes.resize(nodeCnt);

    int bvhRootIndex = 0;
    curTriIndex = 0;

    for (size_t i = 0; i < meshes.size(); i++) {
        IObject* mesh = meshes[i].get();
        curNode = bvhRootIndex;

        bvhRootStartIndices.push_back(bvhRootIndex);
        bvhRootIndex += mesh->GetBVH()->m_nodecnt;

        ProcessBLASNodes(mesh->GetBVH()->m_root_idx, *mesh->GetBVH());
        curTriIndex += mesh->GetBVH()->GetNumIndices();
    }
}

///////////////////////////////////////////////////////////////////////////////
void BoundingTranslator::ProcessTLAS(const BoundingHierarchy& topLevelBvh)
{
    curNode = topLevelIndex;
    ProcessTLASNodes(topLevelBvh.m_root_idx, topLevelBvh);
}

///////////////////////////////////////////////////////////////////////////////
void BoundingTranslator::UpdateTLAS(
    const BoundingHierarchy& topLevelBvh,
    const std::vector<Ray::Instance>& sceneInstances)
{
    // this->topLevelBvh = topLevelBvh;
    meshInstances = sceneInstances;
    curNode = topLevelIndex;
    ProcessTLASNodes(topLevelBvh.m_root_idx, topLevelBvh);
}

///////////////////////////////////////////////////////////////////////////////
void BoundingTranslator::Process(const BoundingHierarchy& topLevelBvh,
    std::vector<UniquePtr<IObject>>& objects,
    const std::vector<Ray::Instance>& sceneInstances)
{
    // this->topLevelBvh = topLevelBvh;
    meshInstances = sceneInstances;
    ProcessBLAS(objects);
    ProcessTLAS(topLevelBvh);
}
///////////////////////////////////////////////////////////////////////////////
int BoundingTranslator::ProcessBLASNodes(int nodeIdx,
    const BoundingHierarchy& bvh)
{
    const BoundingHierarchy::Node& node = bvh.m_nodes[nodeIdx];
    Ray::BoundingBox boundingBox = node.bounds;

    nodes[curNode].bboxmin = boundingBox.min;
    nodes[curNode].bboxmax = boundingBox.max;
    nodes[curNode].LRLeaf.z = 0;

    int index = curNode;

    if (node.type == Ray::BoundingHierarchy::NodeType::kLeaf) {
        nodes[curNode].LRLeaf.x = curTriIndex + node.startidx;
        nodes[curNode].LRLeaf.y = node.numprims;
        nodes[curNode].LRLeaf.z = 1;
    } else {
        curNode++;
        nodes[index].LRLeaf.x = ProcessBLASNodes(node.left_child_idx, bvh);
        curNode++;
        nodes[index].LRLeaf.y = ProcessBLASNodes(node.right_child_idx, bvh);
    }
    return index;
}

int BoundingTranslator::ProcessTLASNodes(int nodeIdx,
    const BoundingHierarchy& bvh)
{
    const BoundingHierarchy::Node& node = bvh.m_nodes[nodeIdx];
    Ray::BoundingBox boundingBox = node.bounds;

    nodes[curNode].bboxmin = boundingBox.min;
    nodes[curNode].bboxmax = boundingBox.max;
    nodes[curNode].LRLeaf.z = 0;

    int index = curNode;

    if (node.type == Ray::BoundingHierarchy::NodeType::kLeaf) {
        int instanceIndex = bvh.m_packed_indices[node.startidx];
        int meshIndex = meshInstances[instanceIndex].objectID;
        int materialID = meshInstances[instanceIndex].materialID;

        nodes[curNode].LRLeaf.x = bvhRootStartIndices[meshIndex];
        nodes[curNode].LRLeaf.y = materialID;
        nodes[curNode].LRLeaf.z = -instanceIndex - 1;
    } else {
        curNode++;
        nodes[index].LRLeaf.x = ProcessTLASNodes(node.left_child_idx, bvh);
        curNode++;
        nodes[index].LRLeaf.y = ProcessTLASNodes(node.right_child_idx, bvh);
    }
    return index;
}



}  // namespace Ray

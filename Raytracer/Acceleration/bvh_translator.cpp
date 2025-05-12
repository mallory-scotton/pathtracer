// FIXME: THE WHOLE FILE
#include <cassert>
#include <iostream>
#include <stack>

#include "Acceleration/bvh_translator.h"

namespace Ray {
int BvhTranslator::ProcessBLASNodes(const Bvh::Node* node) {
    Ray::BoundingBox BoundingBox = node->bounds;

    nodes[curNode].bboxmin = BoundingBox.min;
    nodes[curNode].bboxmax = BoundingBox.max;
    nodes[curNode].LRLeaf.z = 0;

    int index = curNode;

    if (node->type == Ray::Bvh::NodeType::kLeaf) {
        nodes[curNode].LRLeaf.x = curTriIndex + node->startidx;
        nodes[curNode].LRLeaf.y = node->numprims;
        nodes[curNode].LRLeaf.z = 1;
    } else {
        curNode++;
        nodes[index].LRLeaf.x = ProcessBLASNodes(node->lc);
        curNode++;
        nodes[index].LRLeaf.y = ProcessBLASNodes(node->rc);
    }
    return index;
}

int BvhTranslator::ProcessTLASNodes(const Bvh::Node* node) {
    Ray::BoundingBox BoundingBox = node->bounds;

    nodes[curNode].bboxmin = BoundingBox.min;
    nodes[curNode].bboxmax = BoundingBox.max;
    nodes[curNode].LRLeaf.z = 0;

    int index = curNode;

    if (node->type == Ray::Bvh::NodeType::kLeaf) {
        int instanceIndex = topLevelBvh->m_packed_indices[node->startidx];
        int meshIndex = meshInstances[instanceIndex].objectID;
        int materialID = meshInstances[instanceIndex].materialID;

        nodes[curNode].LRLeaf.x = bvhRootStartIndices[meshIndex];
        nodes[curNode].LRLeaf.y = materialID;
        nodes[curNode].LRLeaf.z = -instanceIndex - 1;
    } else {
        curNode++;
        nodes[index].LRLeaf.x = ProcessTLASNodes(node->lc);
        curNode++;
        nodes[index].LRLeaf.y = ProcessTLASNodes(node->rc);
    }
    return index;
}

void BvhTranslator::ProcessBLAS(std::vector<UniquePtr<IObject>>& meshes) {
    int nodeCnt = 0;

    for (int i = 0; i < meshes.size(); i++)
        nodeCnt += meshes[i]->GetBVH()->m_nodecnt;
    topLevelIndex = nodeCnt;

    // reserve space for top level nodes
    nodeCnt += 2 * meshInstances.size();
    nodes.resize(nodeCnt);

    int bvhRootIndex = 0;
    curTriIndex = 0;

    for (int i = 0; i < meshes.size(); i++) {
        IObject* mesh = meshes[i].get();
        curNode = bvhRootIndex;

        bvhRootStartIndices.push_back(bvhRootIndex);
        bvhRootIndex += mesh->GetBVH()->m_nodecnt;

        ProcessBLASNodes(mesh->GetBVH()->m_root);
        curTriIndex += mesh->GetBVH()->GetNumIndices();
    }
}

void BvhTranslator::ProcessTLAS(void) {
    curNode = topLevelIndex;
    ProcessTLASNodes(topLevelBvh->m_root);
}

void BvhTranslator::UpdateTLAS(
    const Bvh* topLevelBvh,
    const std::vector<Instance>& sceneInstances) {
    this->topLevelBvh = topLevelBvh;
    meshInstances = sceneInstances;
    curNode = topLevelIndex;
    ProcessTLASNodes(topLevelBvh->m_root);
}

void BvhTranslator::Process(
    const Bvh* topLevelBvh, std::vector<UniquePtr<IObject>>& objects,
    const std::vector<Instance>& sceneInstances) {
    this->topLevelBvh = topLevelBvh;
    meshInstances = sceneInstances;
    ProcessBLAS(objects);
    ProcessTLAS();
}
}  // namespace Ray

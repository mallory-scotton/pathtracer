// FIXME: THE WHOLE FILE
#pragma once

#ifndef BVH_TRANSLATOR_H
#define BVH_TRANSLATOR_H

#include <map>

#include "Components/Mesh.hpp"
#include "Acceleration/bvh.h"

namespace Ray {
class BvhTranslator {
   public:
    // Constructor
    BvhTranslator() = default;

    struct Node {
        Vec3f bboxmin;
        Vec3f bboxmax;
        Vec3f LRLeaf;
    };

    void ProcessBLAS();
    void ProcessTLAS();
    void UpdateTLAS(const Bvh* topLevelBvh,
                    const std::vector<Ray::MeshInstance>& instances);
    void Process(const Bvh* topLevelBvh, const std::vector<Ray::Mesh*>& meshes,
                 const std::vector<Ray::MeshInstance>& instances);
    int topLevelIndex = 0;
    std::vector<Node> nodes;
    int nodeTexWidth;

   private:
    int curNode = 0;
    int curTriIndex = 0;
    std::vector<int> bvhRootStartIndices;
    int ProcessBLASNodes(const Bvh::Node* root);
    int ProcessTLASNodes(const Bvh::Node* root);
    std::vector<Ray::MeshInstance> meshInstances;
    std::vector<Ray::Mesh*> meshes;
    const Bvh* topLevelBvh;
};
}  // namespace Ray

#endif  // BVH_TRANSLATOR_H

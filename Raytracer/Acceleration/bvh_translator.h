// FIXME: THE WHOLE FILE
#pragma once

#ifndef BVH_TRANSLATOR_H
#define BVH_TRANSLATOR_H

#include <map>

#include "Objects/Instance.hpp"
#include "Interfaces/IObject.hpp"
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
                    const std::vector<Ray::Instance>& instances);
    void Process(const Bvh* topLevelBvh, const std::vector<IObject*>& objects,
                 const std::vector<Ray::Instance>& instances);
    int topLevelIndex = 0;
    std::vector<Node> nodes;
    int nodeTexWidth;

   private:
    int curNode = 0;
    int curTriIndex = 0;
    std::vector<int> bvhRootStartIndices;
    int ProcessBLASNodes(const Bvh::Node* root);
    int ProcessTLASNodes(const Bvh::Node* root);
    std::vector<Instance> meshInstances;
    std::vector<IObject*> meshes;
    const Bvh* topLevelBvh;
};
}  // namespace Ray

#endif  // BVH_TRANSLATOR_H

// FIXME: THE WHOLE FILE
#pragma once

#ifndef BVH_H
#define BVH_H

#include <atomic>
#include <iostream>
#include <list>
#include <memory>
#include <vector>

#include "Acceleration/BoundingBox.hpp"

namespace Ray {
///< The class represents bounding volume hierarachy
///< intersection accelerator
///<
class Bvh {
   public:
    Bvh(float traversal_cost, int num_bins = 64, bool usesah = false)
        : m_root(nullptr)
        ,  m_usesah(usesah)
        ,  m_height(0)
        ,  m_traversal_cost(traversal_cost)
        ,  m_num_bins(num_bins)
    {}

    ~Bvh() = default;

    // World space bounding box
    BoundingBox const& Bounds() const;

    // Build function
    // bounds is an array of bounding boxes
    void Build(BoundingBox const* bounds, int numbounds);

    // Get tree height
    int GetHeight() const;

    // Get reordered prim indices Nodes are pointing to
    virtual int const* GetIndices() const;

    // Get number of indices.
    // This number can differ from numbounds passed to Build function for
    // some BVH implementations (like SBVH)
    virtual size_t GetNumIndices() const;

   protected:
    // Build function
    virtual void BuildImpl(BoundingBox const* bounds, int numbounds);
    // BVH node
    struct Node;
    // Node allocation
    virtual Node* AllocateNode();
    virtual void InitNodeAllocator(size_t maxnum);

    struct SplitRequest {
        // Starting index of a request
        int startidx;
        // Number of primitives
        int numprims;
        // Root node
        Node** ptr;
        // Bounding box
        BoundingBox bounds;
        // Centroid bounds
        BoundingBox centroid_bounds;
        // Level
        int level;
        // Node index
        int index;
    };

    struct SahSplit {
        int dim;
        float split;
        float sah;
        float overlap;
    };

    void BuildNode(SplitRequest const& req, BoundingBox const* bounds,
                   Vec3f const* centroids, int* primindices);

    SahSplit FindSahSplit(SplitRequest const& req, BoundingBox const* bounds,
                          Vec3f const* centroids, int* primindices) const;

    // Enum for node type
    enum NodeType { kInternal, kLeaf };

    // Bvh nodes
    std::vector<Node> m_nodes;
    // Identifiers of leaf primitives
    std::vector<int> m_indices;
    // Node allocator counter, atomic for thread safety
    std::atomic<int> m_nodecnt;

    // Identifiers of leaf primitives
    std::vector<int> m_packed_indices;

    // Bounding box containing all primitives
    BoundingBox m_bounds;
    // Root node
    Node* m_root;
    // SAH flag
    bool m_usesah;
    // Tree height
    int m_height;
    // Node traversal cost
    float m_traversal_cost;
    // Number of spatial bins to use for SAH
    int m_num_bins;

   private:
    Bvh(Bvh const&) = delete;
    Bvh& operator=(Bvh const&) = delete;

    friend class BvhTranslator;
};

struct Bvh::Node {
    // Node bounds in world space
    BoundingBox bounds;
    // Type of the node
    NodeType type;
    // Node index in a complete tree
    int index;

    union {
        // For internal nodes: left and right children
        struct {
            Node* lc;
            Node* rc;
        };

        // For leaves: starting primitive index and number of primitives
        struct {
            int startidx;
            int numprims;
        };
    };
};

inline int const* Bvh::GetIndices() const { return &m_packed_indices[0]; }

inline size_t Bvh::GetNumIndices() const { return m_packed_indices.size(); }

inline int Bvh::GetHeight() const { return m_height; }
}  // namespace Ray

#endif  // BVH_H

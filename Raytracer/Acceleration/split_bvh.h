// FIXME: THE WHOLE FILE
#pragma once

#include "Acceleration/bvh.h"

namespace Ray {
class SplitBvh : public Bvh {
   public:
    SplitBvh(float traversal_cost, int num_bins, int max_split_depth,
             float min_overlap, float extra_refs_budget)
        : Bvh(traversal_cost, num_bins, true),
          m_max_split_depth(max_split_depth),
          m_min_overlap(min_overlap),
          m_extra_refs_budget(extra_refs_budget),
          m_num_nodes_required(0),
          m_num_nodes_for_regular(0),
          m_num_nodes_archived(0) {}

    ~SplitBvh() = default;

   protected:
    struct PrimRef;
    using PrimRefArray = std::vector<PrimRef>;

    enum class SplitType { kObject, kSpatial };

    // Build function
    void BuildImpl(BoundingBox const* bounds, int numbounds) override;
    void BuildNode(SplitRequest& req, PrimRefArray& primrefs);

    SahSplit FindObjectSahSplit(SplitRequest const& req,
                                PrimRefArray const& refs) const;
    SahSplit FindSpatialSahSplit(SplitRequest const& req,
                                 PrimRefArray const& refs) const;

    void SplitPrimRefs(SahSplit const& split, SplitRequest const& req,
                       PrimRefArray& refs, int& extra_refs);
    bool SplitPrimRef(PrimRef const& ref, int axis, float split,
                      PrimRef& leftref, PrimRef& rightref) const;

    // Print BVH statistics
    void PrintStatistics(std::ostream& os) const override;

   protected:
    Node* AllocateNode() override;
    void InitNodeAllocator(size_t maxnum) override;

   private:
    int m_max_split_depth;
    float m_min_overlap;
    float m_extra_refs_budget;
    int m_num_nodes_required;
    int m_num_nodes_for_regular;

    // Node archive for memory management
    // As m_nodes fills up we archive it into m_node_archive
    // allocate new chunk and work there.

    // How many nodes have been archived so far
    int m_num_nodes_archived;
    // Container for archived chunks
    std::list<std::vector<Node>> m_node_archive;

    SplitBvh(SplitBvh const&) = delete;
    SplitBvh& operator=(SplitBvh const&) = delete;
};

struct SplitBvh::PrimRef {
    // Prim bounds
    BoundingBox bounds;
    Vec3f center;
    int idx;
};

}  // namespace Ray

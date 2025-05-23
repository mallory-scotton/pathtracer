///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <atomic>
#include <iostream>
#include <list>
#include <memory>
#include <vector>
#include "Acceleration/BoundingBox.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class BoundingHierarchy
{
public:
    ///////////////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////////////
    enum NodeType
    {
        kInternal,
        kLeaf
    }; //<!
    struct Node
    {
        BoundingBox bounds;
        NodeType type;
        int index;
        union
        {
            struct
            {
                int left_child_idx;
                int right_child_idx;
            };
            struct
            {
                int startidx;
                int numprims;
            };
        };
    };

    struct SplitRequest
    {
        int startidx;                   //<!
        int numprims;                   //<!
        int* node_idx;                  // Changed: Store index instead of pointer
        BoundingBox bounds;             //<!
        BoundingBox centroid_bounds;    //<!
        int level;                      //<!
        int index;                      //<!
    };
    struct SahSplit
    {
        int dim;                        //<!
        float split;                    //<!
        float sah;                      //<!
        float overlap;                  //<!
    };
    int m_root_idx;                     //<!
    std::vector<Node> m_nodes;          //<!
    std::vector<int> m_packed_indices;  //<!
    std::atomic<int> m_nodecnt;         //<!

protected:
    ///////////////////////////////////////////////////////////////////////////
    ///
    ///////////////////////////////////////////////////////////////////////////
    std::vector<int> m_indices;         //<!
    BoundingBox m_bounds;               //<!
    bool m_usesah;                      //<!
    int m_height;                       //<!
    float m_traversal_cost;             //<!
    int m_num_bins;                     //<!

public:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param traversal_cost
    /// \param num_bins
    /// \param usesah
    ///
    ///////////////////////////////////////////////////////////////////////////
    BoundingHierarchy(float traversal_cost, int num_bins = 64, bool usesah = false);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    ~BoundingHierarchy();

public:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    BoundingBox const& Bounds() const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param bounds
    /// \param numbounds
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Build(const std::vector<BoundingBox>& bounds, int numbounds);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    int GetHeight() const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual int const* GetIndices() const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual size_t GetNumIndices() const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param v
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool IsNan(float v) const;

protected:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param bounds
    /// \param numbounds
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void BuildImpl(const std::vector<BoundingBox>& bounds, int numbounds);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual int AllocateNode(); // Changed: Return index instead of reference

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param maximum
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void InitNodeAllocator(size_t maxnum);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param req
    /// \param bounds
    /// \param centroids
    /// \param primindices
    ///
    ///////////////////////////////////////////////////////////////////////////
    void BuildNode(const SplitRequest& req,
        const std::vector<BoundingBox>& bounds,
        const std::vector<Vec3f>& centroids,
        std::vector<int>& primindices);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param req
    /// \param bounds
    /// \param centroids
    /// \param primindices
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    SahSplit FindSahSplit(const SplitRequest& req,
        const std::vector<BoundingBox>& bounds,
        const std::vector<Vec3f>& centroids,
        std::vector<int>& primindices) const;
};

}  // namespace Ray
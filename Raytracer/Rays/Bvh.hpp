///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Rays/BBox.hpp"
#include "Config.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class Bvh
{
public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct Node
    {
    public:
        ///////////////////////////////////////////////////////////////////////
        /// \brief
        ///
        ///////////////////////////////////////////////////////////////////////
        enum Type
        {
            INTERNAL,
            LEAF
        };

    public:
        ///////////////////////////////////////////////////////////////////////
        //
        ///////////////////////////////////////////////////////////////////////
        BBox bounds;                //<!
        Type type;                  //<!
        int index;                  //<!
        union
        {
            struct
            {
                Node* left;         //<!
                Node* right;        //<!
            };

            struct
            {
                int startIndex;     //<!
                int numPrims;       //<!
            };
        };
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct SplitRequest
    {
        int startIdx;               //<!
        int numPrims;               //<!
        Node** ptr;                 //<!
        BBox bounds;                //<!
        BBox centroidBounds;        //<!
        int level;                  //<!
        int index;                  //<!
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct SahSplit
    {
        int dimension;              //<!
        float split;                //<!
        float sah;                  //<!
        float overlap;              //<!
    };

private:
    ///////////////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////////////
    Vector<Bvh::Node> mNodes;       //<!
    Vector<int> mIndices;           //<!
    Atomic<int> mNodeCount;         //<!
    Vector<int> mPackedIndices;     //<!
    BBox mBounds;                   //<!
    Node* mRoot;                    //<!
    bool mUseSAH;                   //<!
    int mHeight;                    //<!
    float mTraversalCost;           //<!
    int mNumBins;                   //<!

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param traversalCost
    /// \param numBins
    /// \param useSAH
    ///
    ///////////////////////////////////////////////////////////////////////////
    Bvh(float traversalCost, int numBins = 64, bool useSAH = false);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual ~Bvh() = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    Bvh(const Bvh&) = delete;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    Bvh& operator=(const Bvh&) = delete;

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    Node* AllocateNode(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param maxNum
    ///
    ///////////////////////////////////////////////////////////////////////////
    void InitNodeAllocator(Uint64 maxNum);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param request
    /// \param bounds
    /// \param centroids
    /// \param primIndices
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    SahSplit FindSahSplit(
        const SplitRequest& request,
        const Vector<BBox>& bounds,
        const Vector<Vec3f>& centroids,
        Vector<int>& primIndices
    ) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param request
    /// \param bounds
    /// \param centroids
    /// \param primIndices
    ///
    ///////////////////////////////////////////////////////////////////////////
    void BuildNode(
        const SplitRequest& request,
        const Vector<BBox>& bounds,
        const Vector<Vec3f>& centroids,
        Vector<int>& primIndices
    );

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    const BBox& GetBounds(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    int GetHeight(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    const Vector<int>& GetIndices(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    const int* GetIndicesData(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    Uint64 GetIndicesCount(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param bounds
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Build(const Vector<BBox>& bounds);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    int GetNodeCount(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    Node* GetRoot(void) const;
};

} // namespace Ray

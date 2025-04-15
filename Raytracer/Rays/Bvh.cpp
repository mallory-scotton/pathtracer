///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Rays/Bvh.hpp"
#include "Maths/Utils.hpp"
#include <numeric>

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
static constexpr int MAX_PRIMITIVES_PER_LEAF = 1;

///////////////////////////////////////////////////////////////////////////////
Bvh::Bvh(float traversalCost, int numBins, bool useSAH)
    : mRoot(nullptr)
    , mUseSAH(useSAH)
    , mHeight(0)
    , mTraversalCost(traversalCost)
    , mNumBins(numBins)
{}

///////////////////////////////////////////////////////////////////////////////
const BBox& Bvh::GetBounds(void) const
{
    return (mBounds);
}

///////////////////////////////////////////////////////////////////////////////
int Bvh::GetHeight(void) const
{
    return (mHeight);
}

///////////////////////////////////////////////////////////////////////////////
const Vector<int>& Bvh::GetIndices(void) const
{
    return (mPackedIndices);
}

///////////////////////////////////////////////////////////////////////////////
const int* Bvh::GetIndicesData(void) const
{
    return (&mPackedIndices[0]);
}

///////////////////////////////////////////////////////////////////////////////
Uint64 Bvh::GetIndicesCount(void) const
{
    return (mPackedIndices.size());
}

///////////////////////////////////////////////////////////////////////////////
void Bvh::InitNodeAllocator(Uint64 maxNum)
{
    mNodeCount = 0;
    mNodes.resize(maxNum);
}

///////////////////////////////////////////////////////////////////////////////
Bvh::Node* Bvh::AllocateNode(void)
{
    return (&mNodes[mNodeCount++]);
}

///////////////////////////////////////////////////////////////////////////////
Bvh::SahSplit Bvh::FindSahSplit(
    const Bvh::SplitRequest& request,
    const Vector<BBox>& bounds,
    const Vector<Vec3f>& centroids,
    Vector<int>& primIndices
) const
{
    int splitIdx = -1;
    float sah = std::numeric_limits<float>::max();
    SahSplit split;

    split.dimension = 0;
    split.split = std::numeric_limits<float>::quiet_NaN();

    Vec3f centroidExtents = request.centroidBounds.Extents();
    if (Vec3f::Dot(centroidExtents, centroidExtents) == 0.f)
    {
        return (split);
    }

    struct Bin
    {
        BBox bounds;
        int count;
    };

    Vector<Bin> bins[3];
    bins[0].resize(mNumBins);
    bins[1].resize(mNumBins);
    bins[2].resize(mNumBins);

    float invArea = 1.f / request.bounds.SurfaceArea();
    Vec3f rootMin = request.centroidBounds.GetMin();

    for (int axis = 0; axis < 3; axis++)
    {
        float rootMinC = rootMin[axis];
        float centroidRng = centroidExtents[axis];
        float invCentroidRng = 1.f / centroidRng;

        if (centroidRng == 0.f)
        {
            continue;
        }

        for (int i = 0; i < mNumBins; i++)
        {
            bins[axis][i].count = 0;
            bins[axis][i].bounds = BBox();
        }

        for (int i = request.startIdx; i < request.startIdx + request.numPrims; i++)
        {
            int idx = primIndices[i];
            int binIdx = static_cast<int>(std::min<float>(
                static_cast<float>(mNumBins) * (
                    (centroids[idx][axis] - rootMinC) * invCentroidRng
                ),
                static_cast<float>(mNumBins - 1)
            ));

            bins[axis][binIdx].count++;
            bins[axis][binIdx].bounds.Grow(bounds[idx]);
        }

        Vector<BBox> rightBounds(mNumBins - 1);

        BBox rightBox;
        for (int i = mNumBins - 1; i > 0; i--)
        {
            rightBox.Grow(bins[axis][i].bounds);
            rightBounds[i - 1] = rightBox;
        }

        BBox leftBox;
        int leftCount = 0;
        int rightCount = request.numPrims;

        float sahTmp = 0.f;
        for (int i = 0; i < mNumBins - 1; i++)
        {
            leftBox.Grow(bins[axis][i].bounds);
            leftCount += bins[axis][i].count;
            rightCount -= bins[axis][i].count;

            sahTmp = mTraversalCost + (
                leftCount * leftBox.SurfaceArea() +
                rightCount * rightBounds[i].SurfaceArea()
            ) * invArea;

            if (sahTmp < sah)
            {
                split.dimension = axis;
                splitIdx = i;
                split.sah = sah = sahTmp;
            }
        }
    }

    if (splitIdx != -1)
    {
        split.split = rootMin[split.dimension] +
            (splitIdx + 1) * (centroidExtents[split.dimension] / mNumBins);
    }

    return (split);
}

///////////////////////////////////////////////////////////////////////////////
void Bvh::BuildNode(
    const SplitRequest& request,
    const Vector<BBox>& bounds,
    const Vector<Vec3f>& centroids,
    Vector<int>& primIndices
)
{
    mHeight = std::max(mHeight, request.level);

    Node* node = AllocateNode();

    node->bounds = request.bounds;
    node->index = request.index;

    if (request.numPrims < 2)
    {
        node->type = Node::Type::LEAF;
        node->startIndex = static_cast<int>(mPackedIndices.size());
        node->numPrims = request.numPrims;

        for (int i = 0; i < request.numPrims; i++)
        {
            mPackedIndices.push_back(primIndices[request.startIdx + i]);
        }
    }
    else
    {
        int axis = request.centroidBounds.MaxDimension();
        float border = request.centroidBounds.Center()[axis];

        if (mUseSAH)
        {
            SahSplit ss = FindSahSplit(
                request, bounds, centroids, primIndices
            );

            if (!Math::IsNaN(ss.split))
            {
                axis = ss.split;
                border = ss.split;

                if (
                    request.numPrims < ss.sah &&
                    request.numPrims < MAX_PRIMITIVES_PER_LEAF
                )
                {
                    node->type = Node::Type::LEAF;
                    node->startIndex = static_cast<int>(mPackedIndices.size());
                    node->numPrims = request.numPrims;

                    for (int i = 0; i < request.numPrims; i++)
                    {
                        mPackedIndices.push_back(
                            primIndices[request.startIdx + i]
                        );
                    }

                    if (request.ptr)
                    {
                        *request.ptr = node;
                    }
                    return;
                }
            }
        }

        node->type = Node::Type::INTERNAL;

        BBox leftBounds, rightBounds, leftCentroidBounds, rightCentroidBounds;
        int splitIdx = request.startIdx;

        bool nearToFar = (request.numPrims + request.startIdx) & 0x1;

        if (request.centroidBounds.Extents()[axis] > 0.f)
        {
            int first = request.startIdx;
            int last = request.startIdx + request.numPrims;

            if (nearToFar)
            {
                while (true)
                {
                    while (
                        (first != last) &&
                        centroids[primIndices[first]][axis] < border
                    )
                    {
                        leftBounds.Grow(bounds[primIndices[first]]);
                        leftCentroidBounds.Grow(centroids[primIndices[first]]);
                        first++;
                    }

                    if (first == last--)
                    {
                        break;
                    }

                    rightBounds.Grow(bounds[primIndices[first]]);
                    rightCentroidBounds.Grow(centroids[primIndices[first]]);

                    while (
                        (first != last) &&
                        centroids[primIndices[last]][axis] >= border
                    )
                    {
                        rightBounds.Grow(bounds[primIndices[last]]);
                        rightCentroidBounds.Grow(centroids[primIndices[last]]);
                        last--;
                    }

                    if (first == last)
                    {
                        break;
                    }

                    leftBounds.Grow(bounds[primIndices[last]]);
                    leftCentroidBounds.Grow(centroids[primIndices[last]]);

                    std::swap(primIndices[first++], primIndices[last]);
                }
            }
            else
            {
                while (true)
                {
                    while (
                        (first != last) &&
                        centroids[primIndices[first]][axis] > border
                    )
                    {
                        leftBounds.Grow(bounds[primIndices[first]]);
                        leftCentroidBounds.Grow(centroids[primIndices[first]]);
                        first++;
                    }

                    if (first == last--)
                    {
                        break;
                    }

                    rightBounds.Grow(bounds[primIndices[first]]);
                    rightCentroidBounds.Grow(centroids[primIndices[first]]);

                    while (
                        (first != last) &&
                        centroids[primIndices[last]][axis] < border
                    )
                    {
                        rightBounds.Grow(bounds[primIndices[last]]);
                        rightCentroidBounds.Grow(centroids[primIndices[last]]);
                        last--;
                    }

                    if (first == last)
                    {
                        break;
                    }

                    leftBounds.Grow(bounds[primIndices[last]]);
                    leftCentroidBounds.Grow(centroids[primIndices[last]]);

                    std::swap(primIndices[first++], primIndices[last]);
                }
            }

            splitIdx = first;
        }

        if (
            splitIdx == request.startIdx ||
            splitIdx == request.startIdx + request.numPrims
        )
        {
            splitIdx = request.startIdx + (request.numPrims >> 1);

            for (int i = request.startIdx; i < splitIdx; i++)
            {
                leftBounds.Grow(bounds[primIndices[i]]);
                leftCentroidBounds.Grow(centroids[primIndices[i]]);
            }

            for (int i = splitIdx; i < request.startIdx + request.numPrims; i++)
            {
                rightBounds.Grow(bounds[primIndices[i]]);
                rightCentroidBounds.Grow(centroids[primIndices[i]]);
            }
        }

        SplitRequest leftRequest = {
            request.startIdx, splitIdx - request.startIdx,
            &node->left, leftBounds, leftCentroidBounds,
            request.level + 1, (request.index << 1)
        };
        SplitRequest rightRequest = {
            splitIdx, request.numPrims - (splitIdx - request.startIdx),
            &node->right, rightBounds, rightCentroidBounds, request.level + 1,
            (request.index << 1) + 1
        };

        {
            BuildNode(leftRequest, bounds, centroids, primIndices);
        }

        {
            BuildNode(rightRequest, bounds, centroids, primIndices);
        }
    }

    if (request.ptr)
    {
        *request.ptr = node;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Bvh::Build(const Vector<BBox>& bounds)
{
    for (const auto& bound : bounds)
    {
        mBounds.Grow(bound);
    }

    InitNodeAllocator(2 * bounds.size() - 1);

    Vector<Vec3f> centroids(bounds.size());
    mIndices.resize(bounds.size());
    std::iota(mIndices.begin(), mIndices.end(), 0);

    BBox centroidBounds;

    for (Uint64 i = 0; i < bounds.size(); i++)
    {
        Vec3f center = bounds[i].Center();
        centroidBounds.Grow(center);
        centroids[i] = center;
    }

    SplitRequest init = {
        0, static_cast<int>(bounds.size()), nullptr,
        mBounds, centroidBounds, 0, 1
    };

    BuildNode(init, bounds, centroids, mIndices);

    mRoot = &mNodes[0];
}

///////////////////////////////////////////////////////////////////////////////
int Bvh::GetNodeCount(void) const
{
    return (mNodeCount);
}

///////////////////////////////////////////////////////////////////////////////
Bvh::Node* Bvh::GetRoot(void) const
{
    return (mRoot);
}

} // namespace Ray

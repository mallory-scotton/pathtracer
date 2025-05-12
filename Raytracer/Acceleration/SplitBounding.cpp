///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <cassert>
#include <cmath>
#include "Acceleration/SplitBounding.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Ray
///////////////////////////////////////////////////////////////////////////////
namespace Ray
{

///////////////////////////////////////////////////////////////////////////////
SplitBvh::SplitBvh(float traversal_cost, int num_bins, int max_split_depth,
                   float min_overlap, float extra_refs_budget)
    : BoundingHierarchy(traversal_cost, num_bins, true)
    , m_max_split_depth(max_split_depth)
    , m_min_overlap(min_overlap)
    , m_extra_refs_budget(extra_refs_budget)
    , m_num_nodes_archived(0)
{}

///////////////////////////////////////////////////////////////////////////////
SplitBvh::~SplitBvh()
{
    for (auto& nodes : m_node_archive) {
        nodes.clear();
    }
    m_node_archive.clear();
}

///////////////////////////////////////////////////////////////////////////////
void SplitBvh::BuildImpl(std::vector<BoundingBox> bounds, int numbounds)
{
    // Initialize prim refs structures
    PrimRefArray primrefs(numbounds);

    // Keep centroids to speed up partitioning
    std::vector<Vec3f> centroids(numbounds);
    BoundingBox centroid_bounds;

    for (auto i = 0; i < numbounds; ++i) {
        primrefs[i] = PrimRef{bounds[i], bounds[i].Center(), i};

        auto c = bounds[i].Center();
        centroid_bounds.Grow(c);
    }

    m_num_nodes_for_regular = (2 * numbounds - 1);
    m_num_nodes_required =
        (int)(m_num_nodes_for_regular * (1.f + m_extra_refs_budget));

    InitNodeAllocator(m_num_nodes_required);

    SplitRequest init = {0, numbounds, nullptr, m_bounds, centroid_bounds, 0};

    // Start from the top
    BuildNode(init, primrefs);
}

///////////////////////////////////////////////////////////////////////////////
void SplitBvh::BuildNode(SplitRequest& req, PrimRefArray primrefs) {
    // Update current height
    m_height = std::max(m_height, req.level);

    // Allocate new node
    int node_idx = AllocateNode();
    Node& node = m_nodes[node_idx];
    node.bounds = req.bounds;
    node.index = req.index;

    // Create leaf node if we have enough prims
    if (req.numprims < 4) {
        node.type = kLeaf;
        node.startidx = (int)m_packed_indices.size();
        node.numprims = req.numprims;

        for (int i = req.startidx; i < req.startidx + req.numprims; ++i) {
            m_packed_indices.push_back(primrefs[i].idx);
        }
    } else {
        node.type = kInternal;

        // Choose the maximum extent
        int axis = req.centroid_bounds.MaxDimension();
        float border = req.centroid_bounds.Center()[axis];

        SahSplit os = FindObjectSahSplit(req, primrefs);
        SahSplit ss;
        auto split_type = SplitType::kObject;

        // Only use split if
        // 1. Maximum depth is not exceeded
        // 2. We found spatial split
        // 3. It is better than object split
        // 4. Object split is not good enough (too much overlap)
        // 5. Our node budget still allows us to split references
        if (req.level < m_max_split_depth && m_nodecnt < m_num_nodes_required &&
            os.overlap > m_min_overlap) {
            ss = FindSpatialSahSplit(req, primrefs);

            if (!IsNan(ss.split) && ss.sah < os.sah) {
                split_type = SplitType::kSpatial;
            }
        }

        if (split_type == SplitType::kSpatial) {
            // First we need maximum 2x numprims elements allocated
            size_t elems = req.startidx + req.numprims * 2;
            if (primrefs.size() < elems) {
                primrefs.resize(elems);
            }

            // Split prim refs and add extra refs to request
            int extra_refs = 0;
            SplitPrimRefs(ss, req, primrefs, extra_refs);
            req.numprims += extra_refs;
            border = ss.split;
            axis = ss.dim;
        } else {
            border = !IsNan(os.split) ? os.split : border;
            axis = !IsNan(os.split) ? os.dim : axis;
        }

        // Start partitioning and updating extents for children at the same time
        BoundingBox leftbounds, rightbounds, leftcentroid_bounds,
            rightcentroid_bounds;
        int splitidx = req.startidx;

        bool near2far = (req.numprims + req.startidx) & 0x1;

        bool (*cmpl)(float, float) = [](float a, float b) -> bool {
            return a < b;
        };
        bool (*cmpge)(float, float) = [](float a, float b) -> bool {
            return a >= b;
        };
        auto cmp1 = near2far ? cmpl : cmpge;
        auto cmp2 = near2far ? cmpge : cmpl;

        if (req.centroid_bounds.Extents()[axis] > 0.f) {
            auto first = req.startidx;
            auto last = req.startidx + req.numprims;

            while (true) {
                while ((first != last) &&
                       cmp1(primrefs[first].center[axis], border)) {
                    leftbounds.Grow(primrefs[first].bounds);
                    leftcentroid_bounds.Grow(primrefs[first].center);
                    ++first;
                }

                if (first == last--) break;

                rightbounds.Grow(primrefs[first].bounds);
                rightcentroid_bounds.Grow(primrefs[first].center);

                while ((first != last) &&
                       cmp2(primrefs[last].center[axis], border)) {
                    rightbounds.Grow(primrefs[last].bounds);
                    rightcentroid_bounds.Grow(primrefs[last].center);
                    --last;
                }

                if (first == last) break;

                leftbounds.Grow(primrefs[last].bounds);
                leftcentroid_bounds.Grow(primrefs[last].center);

                std::swap(primrefs[first++], primrefs[last]);
            }

            splitidx = first;
        }

        if (splitidx == req.startidx ||
            splitidx == req.startidx + req.numprims) {
            splitidx = req.startidx + (req.numprims >> 1);

            for (int i = req.startidx; i < splitidx; ++i) {
                leftbounds.Grow(primrefs[i].bounds);
                leftcentroid_bounds.Grow(primrefs[i].center);
            }

            for (int i = splitidx; i < req.startidx + req.numprims; ++i) {
                rightbounds.Grow(primrefs[i].bounds);
                rightcentroid_bounds.Grow(primrefs[i].center);
            }
        }

        // Allocate indices for children
        int left_child_idx = 0;
        int right_child_idx = 0;

        // Left request
        SplitRequest leftrequest = {
            req.startidx, splitidx - req.startidx, &left_child_idx,
            leftbounds,   leftcentroid_bounds,     req.level + 1, node_idx * 2 + 1};
        // Right request
        SplitRequest rightrequest = {splitidx,
                                     req.numprims - (splitidx - req.startidx),
                                     &right_child_idx,
                                     rightbounds,
                                     rightcentroid_bounds,
                                     req.level + 1,
                                     node_idx * 2 + 2};

        // The order is very important here since right node uses the space at
        // the end of the array to partition
        { BuildNode(rightrequest, primrefs); }

        {
            // Put those to stack
            BuildNode(leftrequest, primrefs);
        }

        // Set child indices
        node.left_child_idx = left_child_idx;
        node.right_child_idx = right_child_idx;
    }

    // Set node index if any
    if (req.node_idx) *req.node_idx = node_idx;

    // If this is root, save root index
    if (req.level == 0) {
        m_root_idx = node_idx;
    }
}

///////////////////////////////////////////////////////////////////////////////
SplitBvh::SahSplit SplitBvh::FindObjectSahSplit(
    const SplitRequest& req, const PrimRefArray refs) const
{
    // SAH implementation
    // calc centroids histogram
    // moving split bin index
    int splitidx = -1;
    // Set SAH to maximum float value as a start
    auto sah = std::numeric_limits<float>::max();
    SahSplit split;
    split.dim = 0;
    split.split = std::numeric_limits<float>::quiet_NaN();
    split.sah = sah;

    // if we cannot apply histogram algorithm
    // put NAN sentinel as split border
    // PerformObjectSplit simply splits in half
    // in this case
    Vec3f centroid_extents = req.centroid_bounds.Extents();
    if (Vec3f::Dot(centroid_extents, centroid_extents) == 0.f) {
        return split;
    }

    // Bin has BoundingBox and occurence count
    struct Bin {
        BoundingBox bounds;
        int count;
    };

    // Keep bins for each dimension
    std::vector<Bin> bins[3];
    bins[0].resize(m_num_bins);
    bins[1].resize(m_num_bins);
    bins[2].resize(m_num_bins);

    // Precompute inverse parent area
    auto invarea = 1.f / req.bounds.SurfaceArea();
    // Precompute min point
    auto rootmin = req.centroid_bounds.min;

    // Evaluate all dimensions
    for (int axis = 0; axis < 3; ++axis) {
        float rootminc = rootmin[axis];
        // Range for histogram
        auto centroid_rng = centroid_extents[axis];
        auto invcentroid_rng = 1.f / centroid_rng;

        // If the box is degenerate in that dimension skip it
        if (centroid_rng == 0.f) continue;

        // Initialize bins
        for (int i = 0; i < m_num_bins; ++i) {
            bins[axis][i].count = 0;
            bins[axis][i].bounds = BoundingBox();
        }

        // Calc primitive refs histogram
        for (int i = req.startidx; i < req.startidx + req.numprims; ++i) {
            auto idx = i;
            auto binidx = (int)std::min<float>(
                static_cast<float>(m_num_bins) *
                    ((refs[idx].center[axis] - rootminc) * invcentroid_rng),
                static_cast<float>(m_num_bins - 1));

            ++bins[axis][binidx].count;
            bins[axis][binidx].bounds.Grow(refs[idx].bounds);
        }

        std::vector<BoundingBox> rightbounds(m_num_bins - 1);

        // Start with 1-bin right box
        BoundingBox rightbox = BoundingBox();
        for (int i = m_num_bins - 1; i > 0; --i) {
            rightbox.Grow(bins[axis][i].bounds);
            rightbounds[i - 1] = rightbox;
        }

        BoundingBox leftbox = BoundingBox();
        int leftcount = 0;
        int rightcount = req.numprims;

        // Start best SAH search
        // i is current split candidate (split between i and i + 1)
        float sahtmp = 0.f;
        for (int i = 0; i < m_num_bins - 1; ++i) {
            leftbox.Grow(bins[axis][i].bounds);
            leftcount += bins[axis][i].count;
            rightcount -= bins[axis][i].count;

            // Compute SAH
            sahtmp =
                m_traversal_cost + (leftcount * leftbox.SurfaceArea() +
                                    rightcount * rightbounds[i].SurfaceArea()) *
                                       invarea;

            // Check if it is better than what we found so far
            if (sahtmp < sah) {
                split.dim = axis;
                splitidx = i;
                sah = sahtmp;

                // Calculate percentage of overlap
                split.overlap =
                    BoundingBox::Intersection(leftbox, rightbounds[i])
                        .SurfaceArea() *
                    invarea;
            }
        }
    }

    // Choose split plane
    if (splitidx != -1) {
        split.split =
            rootmin[split.dim] +
            (splitidx + 1) * (centroid_extents[split.dim] / m_num_bins);
        split.sah = sah;
    }

    return split;
}

///////////////////////////////////////////////////////////////////////////////
SplitBvh::SahSplit SplitBvh::FindSpatialSahSplit(
    const SplitRequest& req, const PrimRefArray refs) const
{
    // SAH implementation
    // calc centroids histogram
    int const kNumBins = 128;
    // Set SAH to maximum float value as a start
    auto sah = std::numeric_limits<float>::max();
    SahSplit split;
    split.dim = 0;
    split.split = std::numeric_limits<float>::quiet_NaN();
    split.sah = sah;

    // Extents
    Vec3f extents = req.bounds.Extents();
    auto invarea = 1.f / req.bounds.SurfaceArea();

    // If there are too few primitives don't split them
    if (Vec3f::Dot(extents, extents) == 0.f) {
        return split;
    }

    // Bin has start and exit counts + bounds
    struct Bin {
        BoundingBox bounds;
        int enter;
        int exit;
    };

    Bin bins[3][kNumBins];

    // Prepcompute some useful stuff
    Vec3f origin = req.bounds.min;
    Vec3f binsize = req.bounds.Extents() * (1.f / kNumBins);
    Vec3f invbinsize = Vec3f(1.f / binsize.x, 1.f / binsize.y, 1.f / binsize.z);

    // Initialize bins
    for (int axis = 0; axis < 3; ++axis) {
        for (int i = 0; i < kNumBins; ++i) {
            bins[axis][i].bounds = BoundingBox();
            bins[axis][i].enter = 0;
            bins[axis][i].exit = 0;
        }
    }

    // Iterate thru all primitive refs
    for (int i = req.startidx; i < req.startidx + req.numprims; ++i) {
        PrimRef const& primref(refs[i]);
        // Determine starting bin for this primitive
        Vec3f firstbin = Vec3f::Clamp((primref.bounds.min - origin) * invbinsize,
                                     Vec3f(0), Vec3f(kNumBins - 1));
        // Determine finishing bin
        Vec3f lastbin = Vec3f::Clamp((primref.bounds.max - origin) * invbinsize,
                                    firstbin, Vec3f(kNumBins - 1));
        // Iterate over axis
        for (int axis = 0; axis < 3; ++axis) {
            // Skip in case of a degenerate dimension
            if (extents[axis] == 0.f) continue;
            // Break the prim into bins
            auto tempref = primref;

            for (int j = (int)firstbin[axis]; j < (int)lastbin[axis]; ++j) {
                PrimRef leftref, rightref;
                // Split primitive ref into left and right
                float splitval = origin[axis] + binsize[axis] * (j + 1);
                if (SplitPrimRef(tempref, axis, splitval, leftref, rightref)) {
                    // Add left one
                    bins[axis][j].bounds.Grow(leftref.bounds);
                    // Save right to add part of it into the next bin
                    tempref = rightref;
                }
            }
            // Add the last piece into the last bin
            bins[axis][(int)lastbin[axis]].bounds.Grow(tempref.bounds);
            // Adjust enter & exit counters
            bins[axis][(int)firstbin[axis]].enter++;
            bins[axis][(int)lastbin[axis]].exit++;
        }
    }

    // Prepare moving window data
    BoundingBox rightbounds[kNumBins - 1];
    split.sah = std::numeric_limits<float>::max();

    // Iterate over axis
    for (int axis = 0; axis < 3; ++axis) {
        // Skip if the extent is degenerate in that direction
        if (extents[axis] == 0.f) continue;

        // Start with 1-bin right box
        BoundingBox rightbox = BoundingBox();
        for (int i = kNumBins - 1; i > 0; --i) {
            rightbox = BoundingBox::Union(rightbox, bins[axis][i].bounds);
            rightbounds[i - 1] = rightbox;
        }

        BoundingBox leftbox = BoundingBox();
        int leftcount = 0;
        int rightcount = req.numprims;

        // Start moving border to the right
        for (int i = 1; i < kNumBins; ++i) {
            // New left box
            leftbox.Grow(bins[axis][i - 1].bounds);
            // New left box count
            leftcount += bins[axis][i - 1].enter;
            // Adjust right box
            rightcount -= bins[axis][i - 1].exit;
            // Calc SAH
            float sah = m_traversal_cost +
                        (leftbox.SurfaceArea() *
                         +rightbounds[i - 1].SurfaceArea() * rightcount) *
                            invarea;

            // Update SAH if it is needed
            if (sah < split.sah) {
                split.sah = sah;
                split.dim = axis;
                split.split = origin[axis] + binsize[axis] * (float)i;

                // For spatial split overlap is zero by design
                split.overlap = 0.f;
            }
        }
    }

    return split;
}

///////////////////////////////////////////////////////////////////////////////
bool SplitBvh::SplitPrimRef(const PrimRef& ref, int axis, float split,
                            PrimRef& leftref, PrimRef& rightref) const
{
    // Start with left and right refs equal to original ref
    leftref.idx = rightref.idx = ref.idx;
    leftref.bounds = rightref.bounds = ref.bounds;

    // Only split if split value is within our bounds range
    if (split > ref.bounds.min[axis] && split < ref.bounds.max[axis]) {
        // Trim left box on the right
        leftref.bounds.max[axis] = split;
        // Trim right box on the left
        rightref.bounds.min[axis] = split;
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
void SplitBvh::SplitPrimRefs(const SahSplit& split, const SplitRequest& req,
                             PrimRefArray refs, int extra_refs)
{
    // We are going to append new primitives at the end of the array
    int appendprims = req.numprims;

    // Split refs if any of them require to be split
    for (int i = req.startidx; i < req.startidx + req.numprims; ++i) {
        assert(static_cast<size_t>(req.startidx + appendprims) < refs.size());

        PrimRef leftref, rightref;
        if (SplitPrimRef(refs[i], split.dim, split.split, leftref, rightref)) {
            // Copy left ref instead of original
            refs[i] = leftref;
            // Append right one at the end
            refs[req.startidx + appendprims++] = rightref;
        }
    }

    // Return number of primitives after this operation
    extra_refs = appendprims - req.numprims;
}

///////////////////////////////////////////////////////////////////////////////
int SplitBvh::AllocateNode() {
    if (m_nodecnt - m_num_nodes_archived >= m_num_nodes_for_regular) {
        m_node_archive.push_back(std::move(m_nodes));
        m_num_nodes_archived += m_num_nodes_for_regular;
        m_nodes = std::vector<Node>(m_num_nodes_for_regular);
    }

    return (m_nodecnt++ - m_num_nodes_archived);
}

///////////////////////////////////////////////////////////////////////////////
void SplitBvh::InitNodeAllocator(size_t maxnum)
{
    m_node_archive.clear();
    m_nodecnt = 0;
    m_nodes.resize(maxnum);

}


}  // namespace Ray

#ifndef UTILS_FRUSTUM_H
#define UTILS_FRUSTUM_H

#include <unordered_map>

#include <utils/clipspace.h>
#include <utils/boundingbox.h>

namespace simplex
{
namespace utils
{

template<typename T>
struct FrustumT;

template<typename T>
FrustumT<T> operator *(const TransformT<FrustumT<T>::length(), T> &t, const FrustumT<T> &f);

template<typename T>
struct FrustumT
{
    static_assert(std::numeric_limits<T>::is_iec559, "The base type of Frustum must be floating point");

public:
    static constexpr size_t length() { return 3u; }
    static constexpr size_t numPoints() { return numberOfKDimFacesOfNDimBox(0ULL, length()); }
    static constexpr size_t numEdges() { return numberOfKDimFacesOfNDimBox(1ULL, length()); }
    static constexpr size_t numPlanes() { return numberOfKDimFacesOfNDimBox(2ULL, length()); }
    using value_type = T;
    using ClipSpaceType = ClipSpaceT<T>;
    using TransformType = TransformT<length(), T>;
    using MatrixType = typename ClipSpaceType::MatrixType;
    using ZRangeType = RangeT<T>;
    using PointType = glm::vec<length(), T>;
    using EdgeType = LineSegmentT<length(), T>;
    using PlaneType = PlaneT<length(), T>;

    ENUMCLASS(PlaneIndex, uint16_t, Left, Right, Bottom, Top, Near, Far)
    static_assert(numPlanes() == numElementsPlaneIndex(), "Number of frutum plane indices is not equal to number of planes of frustum");

    using Points = std::unordered_map<size_t, PointType>;
    using Edges = std::unordered_map<size_t, EdgeType>;
    using Planes = std::unordered_map<PlaneIndex, PlaneType>;

    FrustumT(const TransformType &viewTransform = TransformType(),
             const ClipSpaceType &clipSpace = ClipSpace(),
             const ZRangeType &zRange = ZRangeType());

    const MatrixType &projectionMatrix() const;
    const TransformType &viewTransform() const;
    const ClipSpaceType &clipSpace() const;
    const ZRangeType &zRange() const;

    const Points &points() const { return m_points; }

    static std::array<size_t, 2u> edgeIndices(size_t);
    const EdgeType &edge(size_t index) const { return m_edges.at(index); }
    const Edges &edges() const { return m_edges; }

    const PlaneType &plane(PlaneIndex index) const { return m_planes.at(index); }
    const Planes &planes() const { return m_planes; }

    int classifyPoint(const PointType &p) const;
    int classifyBoundingBox(const BoundingBoxT<length(), T> &bb) const; // 0 - intersect, +1 - inside, -1 - outside

protected:
    TransformType m_viewTransform;
    ClipSpaceType m_clipSpace;
    ZRangeType m_zRange;
    MatrixType m_projectionMatrix;
    Points m_points;
    Edges m_edges;
    Planes m_planes;

    template<typename T>
    friend FrustumT<T> operator *(const TransformT<FrustumT<T>::length(), T> &t, const FrustumT<T> &f);

};

template<typename T>
inline FrustumT<T>::FrustumT(const TransformType &viewTransform, const ClipSpaceType &clipSpace, const ZRangeType &zRange)
    : m_viewTransform(viewTransform)
    , m_clipSpace(clipSpace)
    , m_zRange(zRange)
{
    static const BoundingBoxT<length(), T> s_boundingBox({ PointType(static_cast<T>(-1)), PointType(static_cast<T>(1)) });

    m_projectionMatrix = m_clipSpace.projectionMatrix(m_zRange);
    const auto vpMatrix = m_projectionMatrix * m_viewTransform;
    const auto vpMatrixInverse = glm::inverse(vpMatrix);

    m_points.reserve(numPoints());
    for (size_t p = 0u; p < numPoints(); ++p)
    {
        auto point = vpMatrixInverse * glm::vec<length() + 1u, T>(s_boundingBox.point(p), static_cast<T>(1));
        m_points.insert({ p, PointType(point / point[length()]) });
    }

    m_edges.reserve(numEdges());
    for (size_t e = 0u; e < numEdges(); ++e)
    {
        auto edgeIndices = s_boundingBox.edgeIndices(e);
        m_edges.insert({ e, EdgeType({ m_points[edgeIndices[0u]], m_points[edgeIndices[1u]] }) });
    }

    m_planes.reserve(numPlanes());
    for (size_t p = 0u; p < numPlanes(); ++p)
    {
        int sign = (p % 2) ? -1 : +1;
        typename PlaneType::BaseType v;
        for (glm::length_t i = 0; i < PlaneType::BaseType::length(); ++i)
            v[i] = vpMatrix[i][length()] + sign * vpMatrix[i][p/2];
        m_planes.insert({ castToPlaneIndex(p), PlaneType(v) });
    }
}

template <typename T>
inline const typename FrustumT<T>::MatrixType &FrustumT<T>::projectionMatrix() const
{
    return m_projectionMatrix;
}

template <typename T>
inline const typename FrustumT<T>::TransformType &FrustumT<T>::viewTransform() const
{
    return m_viewTransform;
}

template<typename T>
inline const typename FrustumT<T>::ClipSpaceType &FrustumT<T>::clipSpace() const
{
    return m_clipSpace;
}

template<typename T>
inline const typename FrustumT<T>::ZRangeType &FrustumT<T>::zRange() const
{
    return m_zRange;
}

template<typename T>
inline std::array<size_t, 2u> FrustumT<T>::edgeIndices(size_t index)
{
    return BoundingBoxT<FrustumT<T>::length(), T>::edgeIndices(index);
}

template <typename T>
inline int FrustumT<T>::classifyPoint(const PointType &p) const
{
    int result = +1;
    for (const auto &[index, plane] : m_planes) {
        if (auto d = plane.distanceTo(p); d < static_cast<T>(0)) {
            result = -1;
            break;
        }
    }
    return result;
}

template <typename T>
inline int FrustumT<T>::classifyBoundingBox(const BoundingBoxT<length(), T> &bb) const
{
    if (bb.isEmpty()) return -1;
    int result = +1;
    for (const auto &[index, plane] : m_planes) {
        if (int c = bb.classifyByPlane(plane); c == -1) {
            result = -1;
            break;
        }
        else if (c == 0)
            result = 0;
    }
    return result;
}

template <typename T>
inline FrustumT<T> operator *(const TransformT<FrustumT<T>::length(), T> &t, const FrustumT<T> &f)
{
    auto result = f;
    result.m_viewTransform *= t.inverted();
    for (auto &[index, p] : result.m_points) p = t * p;
    for (auto &[index, e] : result.m_edges) e = t * e;
    for (auto &[index, p] : result.m_planes) p = t * p;
    return result;
}

} // namespace
} // namespace

#endif // UTILS_FRUSTUM_H

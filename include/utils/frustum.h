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

    const PointType& point(size_t index) const { return m_points.at(index); }
    const Points &points() const { return m_points; }

    static std::array<size_t, 2u> edgeIndices(size_t);
    EdgeType edge(size_t index) const;

    const PlaneType &plane(PlaneIndex index) const { return m_planes.at(index); }
    const Planes &planes() const { return m_planes; }

    int classifyPoint(const PointType &p) const;
    int classifyBoundingBox(const BoundingBoxT<length(), T> &bb) const; // 0 - intersect, +1 - inside, -1 - outside

    PointType closestPoint(const PointType& p) const;

protected:
    TransformType m_viewTransform;
    ClipSpaceType m_clipSpace;
    ZRangeType m_zRange;
    MatrixType m_projectionMatrix;
    Points m_points;
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
    const auto vpMatrixInverted = glm::inverse(vpMatrix);

    m_points.reserve(numPoints());
    for (size_t p = 0u; p < numPoints(); ++p)
    {
        auto point = vpMatrixInverted * glm::vec<length() + 1u, T>(s_boundingBox.point(p), static_cast<T>(1));
        m_points.insert({ p, PointType(point / point[length()]) });
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

template<typename T>
inline typename FrustumT<T>::EdgeType FrustumT<T>::edge(size_t index) const
{
    auto indices = edgeIndices(index);
    return EdgeType({ point(indices[0u]), point(indices[1u]) });
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
inline typename FrustumT<T>::PointType FrustumT<T>::closestPoint(const FrustumT<T>::PointType& p) const
{
    auto test = m_viewTransform.transformPoint(p);

    bool rSignChange = false;
    if (test[0] < static_cast<value_type>(0))
    {
        rSignChange = true;
        test[0] = -test[0];
    }

    bool uSignChange = false;
    if (test[1] < static_cast<value_type>(0))
    {
        uSignChange = true;
        test[1] = -test[1];
    }

    test[2u] *= -1;

    auto pojectionMatrixInverted = glm::inverse(m_projectionMatrix);

    auto minBound = pojectionMatrixInverted * glm::vec4(1.f, 1.f, -1.f, 1.f);
    minBound /= minBound[3u];

    auto maxBound = pojectionMatrixInverted * glm::vec4(1.f, 1.f, 1.f, 1.f);
    maxBound /= maxBound[3u];

    auto fDRatio = maxBound[2u] / minBound[2u];

    auto rmin = minBound[0u];
    auto rmax = maxBound[0u];
    auto umin = minBound[1u];
    auto umax = maxBound[1u];
    auto dmin = -minBound[2u];
    auto dmax = -maxBound[2u];
    auto rminSqr = rmin * rmin;
    auto uminSqr = umin * umin;
    auto dminSqr = dmin * dmin;
    auto minRDDot = rminSqr + dminSqr;
    auto minUDDot = uminSqr + dminSqr;
    auto minRUDDot = rminSqr + minUDDot;
    auto maxRDDot = fDRatio * minRDDot;
    auto maxUDDot = fDRatio * minUDDot;
    auto maxRUDDot = fDRatio * minRUDDot;

    PointType closest;
    value_type rDot, uDot, rdDot, udDot, rudDot, rEdgeDot, uEdgeDot, t;
    if (test[2] >= dmax)
    {
        if (test[0] <= rmax)
        {
            if (test[1] <= umax)
            {
                // F-face
                closest[0] = test[0];
                closest[1] = test[1];
                closest[2] = dmax;
            }
            else
            {
                // UF-edge
                closest[0] = test[0];
                closest[1] = umax;
                closest[2] = dmax;
            }
        }
        else
        {
            if (test[1] <= umax)
            {
                // LF-edge
                closest[0] = rmax;
                closest[1] = test[1];
                closest[2] = dmax;
            }
            else
            {
                // LUF-vertex
                closest[0] = rmax;
                closest[1] = umax;
                closest[2] = dmax;
            }
        }
    }
    else if (test[2] <= dmin)
    {
        if (test[0] <= rmin)
        {
            if (test[1] <= umin)
            {
                // N-face
                closest[0] = test[0];
                closest[1] = test[1];
                closest[2] = dmin;
            }
            else
            {
                udDot = umin * test[1] + dmin * test[2];
                if (udDot >= maxUDDot)
                {
                    // UF-edge
                    closest[0] = test[0];
                    closest[1] = umax;
                    closest[2] = dmax;
                }
                else if (udDot >= minUDDot)
                {
                    // U-face
                    uDot = dmin * test[1] - umin * test[2];
                    t = uDot / minUDDot;
                    closest[0] = test[0];
                    closest[1] = test[1] - t * dmin;
                    closest[2] = test[2] + t * umin;
                }
                else
                {
                    // UN-edge
                    closest[0] = test[0];
                    closest[1] = umin;
                    closest[2] = dmin;
                }
            }
        }
        else
        {
            if (test[1] <= umin)
            {
                rdDot = rmin * test[0] + dmin * test[2];
                if (rdDot >= maxRDDot)
                {
                    // LF-edge
                    closest[0] = rmax;
                    closest[1] = test[1];
                    closest[2] = dmax;
                }
                else if (rdDot >= minRDDot)
                {
                    // L-face
                    rDot = dmin * test[0] - rmin * test[2];
                    t = rDot / minRDDot;
                    closest[0] = test[0] - t * dmin;
                    closest[1] = test[1];
                    closest[2] = test[2] + t * rmin;
                }
                else
                {
                    // LN-edge
                    closest[0] = rmin;
                    closest[1] = test[1];
                    closest[2] = dmin;
                }
            }
            else
            {
                rudDot = rmin * test[0] + umin * test[1] + dmin * test[2];
                rEdgeDot = umin * rudDot - minRUDDot * test[1];
                if (rEdgeDot >= (T)0)
                {
                    rdDot = rmin * test[0] + dmin * test[2];
                    if (rdDot >= maxRDDot)
                    {
                        // LF-edge
                        closest[0] = rmax;
                        closest[1] = test[1];
                        closest[2] = dmax;
                    }
                    else if (rdDot >= minRDDot)
                    {
                        // L-face
                        rDot = dmin * test[0] - rmin * test[2];
                        t = rDot / minRDDot;
                        closest[0] = test[0] - t * dmin;
                        closest[1] = test[1];
                        closest[2] = test[2] + t * rmin;
                    }
                    else
                    {
                        // LN-edge
                        closest[0] = rmin;
                        closest[1] = test[1];
                        closest[2] = dmin;
                    }
                }
                else
                {
                    uEdgeDot = rmin * rudDot - minRUDDot * test[0];
                    if (uEdgeDot >= (T)0)
                    {
                        udDot = umin * test[1] + dmin * test[2];
                        if (udDot >= maxUDDot)
                        {
                            // UF-edge
                            closest[0] = test[0];
                            closest[1] = umax;
                            closest[2] = dmax;
                        }
                        else if (udDot >= minUDDot)
                        {
                            // U-face
                            uDot = dmin * test[1] - umin * test[2];
                            t = uDot / minUDDot;
                            closest[0] = test[0];
                            closest[1] = test[1] - t * dmin;
                            closest[2] = test[2] + t * umin;
                        }
                        else
                        {
                            // UN-edge
                            closest[0] = test[0];
                            closest[1] = umin;
                            closest[2] = dmin;
                        }
                    }
                    else
                    {
                        if (rudDot >= maxRUDDot)
                        {
                            // LUF-vertex
                            closest[0] = rmax;
                            closest[1] = umax;
                            closest[2] = dmax;
                        }
                        else if (rudDot >= minRUDDot)
                        {
                            // LU-edge
                            t = rudDot / minRUDDot;
                            closest[0] = t * rmin;
                            closest[1] = t * umin;
                            closest[2] = t * dmin;
                        }
                        else
                        {
                            // LUN-vertex
                            closest[0] = rmin;
                            closest[1] = umin;
                            closest[2] = dmin;
                        }
                    }
                }
            }
        }
    }
    else
    {
        rDot = dmin * test[0] - rmin * test[2];
        uDot = dmin * test[1] - umin * test[2];
        if (rDot <= (T)0)
        {
            if (uDot <= (T)0)
            {
                // point inside frustum
                closest = test;
            }
            else
            {
                udDot = umin * test[1] + dmin * test[2];
                if (udDot >= maxUDDot)
                {
                    // UF-edge
                    closest[0] = test[0];
                    closest[1] = umax;
                    closest[2] = dmax;
                }
                else
                {
                    // U-face
                    t = uDot / minUDDot;
                    closest[0] = test[0];
                    closest[1] = test[1] - t * dmin;
                    closest[2] = test[2] + t * umin;
                }
            }
        }
        else
        {
            if (uDot <= (T)0)
            {
                rdDot = rmin * test[0] + dmin * test[2];
                if (rdDot >= maxRDDot)
                {
                    // LF-edge
                    closest[0] = rmax;
                    closest[1] = test[1];
                    closest[2] = dmax;
                }
                else
                {
                    // L-face
                    t = rDot / minRDDot;
                    closest[0] = test[0] - t * dmin;
                    closest[1] = test[1];
                    closest[2] = test[2] + t * rmin;
                }
            }
            else
            {
                rudDot = rmin * test[0] + umin * test[1] + dmin * test[2];
                rEdgeDot = umin * rudDot - minRUDDot * test[1];
                if (rEdgeDot >= (T)0)
                {
                    rdDot = rmin * test[0] + dmin * test[2];
                    if (rdDot >= maxRDDot)
                    {
                        // LF-edge
                        closest[0] = rmax;
                        closest[1] = test[1];
                        closest[2] = dmax;
                    }
                    else // assert( rdDot >= minRDDot )
                    {
                        // L-face
                        t = rDot / minRDDot;
                        closest[0] = test[0] - t * dmin;
                        closest[1] = test[1];
                        closest[2] = test[2] + t * rmin;
                    }
                }
                else
                {
                    uEdgeDot = rmin * rudDot - minRUDDot * test[0];
                    if (uEdgeDot >= (T)0)
                    {
                        udDot = umin * test[1] + dmin * test[2];
                        if (udDot >= maxUDDot)
                        {
                            // UF-edge
                            closest[0] = test[0];
                            closest[1] = umax;
                            closest[2] = dmax;
                        }
                        else // assert( udDot >= minUDDot )
                        {
                            // U-face
                            t = uDot / minUDDot;
                            closest[0] = test[0];
                            closest[1] = test[1] - t * dmin;
                            closest[2] = test[2] + t * umin;
                        }
                    }
                    else
                    {
                        if (rudDot >= maxRUDDot)
                        {
                            // LUF-vertex
                            closest[0] = rmax;
                            closest[1] = umax;
                            closest[2] = dmax;
                        }
                        else // assert( rudDot >= minRUDDot )
                        {
                            // LU-edge
                            t = rudDot / minRUDDot;
                            closest[0] = t * rmin;
                            closest[1] = t * umin;
                            closest[2] = t * dmin;
                        }
                    }
                }
            }
        }
    }

    if (rSignChange)
        closest[0] = -closest[0];
    if (uSignChange)
        closest[1] = -closest[1];
    closest[2u] *= -1;

    return m_viewTransform.inverted().transformPoint(closest);
}

template <typename T>
inline FrustumT<T> operator *(const TransformT<FrustumT<T>::length(), T> &t, const FrustumT<T> &f)
{
    auto result = f;
    result.m_viewTransform *= t.inverted();
    for (auto &[index, p] : result.m_points) p = t * p;
    for (auto &[index, p] : result.m_planes) p = t * p;
    return result;
}

} // namespace
} // namespace

#endif // UTILS_FRUSTUM_H

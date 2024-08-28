#ifndef UTILS_BOUNDINGBOX_H
#define UTILS_BOUNDINGBOX_H

#include <vector>
#include <array>
#include <bitset>

#include <utils/glm/detail/type_vec1.hpp>
#include <utils/glm/detail/type_vec2.hpp>
#include <utils/glm/detail/type_vec3.hpp>
#include <utils/forwarddecl.h>
#include <utils/combinatorics.h>
#include <utils/range.h>
#include <utils/transform.h>
#include <utils/plane.h>
#include <utils/line.h>
#include <utils/linesegment.h>

namespace simplex
{
namespace utils
{

template <glm::length_t L, typename T>
struct BoundingBoxT
{
    static_assert((L >= 1) && (L < 4), "The dimaension of BoundingBox must be [1..3]");
    static_assert(std::numeric_limits<T>::is_iec559, "The base type of BoundingBox must be floating point");

public:
    static constexpr size_t length() { return L; }
    static constexpr size_t numPoints() { return numberOfKDimFacesOfNDimBox(0ULL, length()); }
    static constexpr size_t numEdges() { return numberOfKDimFacesOfNDimBox(1ULL, length()); }
    static constexpr size_t numFaces() { return numberOfKDimFacesOfNDimBox(2ULL, length()); }
    using value_type = T;
    using PointType = glm::vec<length(), T>;
    using EdgeType = LineSegmentT<length(), T>;

    BoundingBoxT() : BoundingBoxT<L, T>(empty()) {}
    BoundingBoxT(const std::array<PointType, 2u> &pp) : m_points(pp) {}

    PointType point(size_t) const;
    std::vector<PointType> points() const;

    static std::array<size_t, 2u> edgeIndices(size_t);
    EdgeType edge(size_t) const;
    std::vector<EdgeType> edges() const;

    const PointType &minPoint() const { return m_points[0u]; }
    const PointType &maxPoint() const { return m_points[1u]; }
    PointType center() const { return static_cast<T>(.5) * (m_points[0u] + m_points[1u]); }
    PointType halfSize() const { return static_cast<T>(.5) * (m_points[1u] - m_points[0u]); }

    bool isEmpty() const;

    int classifyPoint(const PointType &p) const; // +1 - inside, -1 - outside
    PointType closestPoint(const PointType &v) const;

    RangeT<T> projectOnLine(const LineT<L, T> &l) const;

    RangeT<T> pairDistancesToPlane(const PlaneT<L, T> &p) const;
    T distanceToPlane(const PlaneT<L, T> &p) const;
    int classifyByPlane(const PlaneT<L, T> &p) const; // 0 - intersect, +1 - in front, -1 - in back

    BoundingBoxT<L, T> &operator += (const BoundingBoxT<L, T> &b);
    BoundingBoxT<L, T> &operator += (const PointType &v);
    BoundingBoxT<L, T> &operator *= (const BoundingBoxT<L, T> &b);

    static BoundingBoxT<L, T> fromMinMax(const PointType &minP, const PointType &maxP) { return BoundingBoxT<L, T>({ minP, maxP }); }
    static BoundingBoxT<L, T> fromCenterHalfSize(const PointType &cP, const PointType &hs) { return BoundingBoxT<L, T>({ cP-hs, cP+hs} ); }
    static BoundingBoxT<L, T> empty() { return BoundingBoxT<L, T>({ PointType(utils::maximum<float>()),
                                                                    PointType(-utils::maximum<float>()) }); }

private:
    std::array<PointType, 2u> m_points;
};

template <glm::length_t L, typename T>
inline BoundingBoxT<L, T> operator +(const BoundingBoxT<L, T> &b1, const BoundingBoxT<L, T> &b2)
{
    return BoundingBoxT<L, T>({ glm::min(b1.minPoint(), b2.minPoint()), glm::max(b1.maxPoint(), b2.maxPoint()) });
}

template <glm::length_t L, typename T>
inline BoundingBoxT<L, T> operator +(const BoundingBoxT<L, T> &b, const typename BoundingBoxT<L, T>::PointType &v)
{
    return BoundingBoxT<L, T>({ glm::min(b.minPoint(), v), glm::max(b.maxPoint(), v) });
}

template <glm::length_t L, typename T>
inline BoundingBoxT<L, T> operator +(const typename BoundingBoxT<L, T>::PointType &v, const BoundingBoxT<L, T> &b)
{
    return b + v;
}

template <glm::length_t L, typename T>
inline BoundingBoxT<L, T> operator *(const BoundingBoxT<L, T> &b1, const BoundingBoxT<L, T> &b2)
{
    return BoundingBoxT<L, T>({ glm::max(b1.minPoint(), b2.minPoint()), glm::min(b1.maxPoint(), b2.maxPoint()) });
}

template <glm::length_t L, typename T>
inline BoundingBoxT<L, T> operator *(const TransformT<L, T> &t, const BoundingBoxT<L, T> &b)
{
    if (b.isEmpty()) return BoundingBoxT<L, T>::empty();

    const auto xAxis = glm::abs(t.rotation * glm::vec3(1.f, 0.f, 0.f));
    const auto yAxis = glm::abs(t.rotation * glm::vec3(0.f, 1.f, 0.f));
    const auto zAxis = glm::abs(t.rotation * glm::vec3(0.f, 0.f, 1.f));

    const auto halfSize = glm::mat<L, L, T>(glm::mat3(xAxis, yAxis, zAxis)) * (b.halfSize() * t.scale);
    const auto center = t * b.center();

    return BoundingBoxT<L, T>({ center - halfSize, center + halfSize });
}

template <glm::length_t L, typename T>
inline BoundingBoxT<L, T> operator *(const glm::mat<L+1, L+1, T> &t, const BoundingBoxT<L, T> &b)
{
    if (b.isEmpty()) return BoundingBoxT<L, T>::empty();

    const auto halfSize = glm::abs(glm::vec<L, T>(t * glm::vec<L+1, T>(b.halfSize(), static_cast<T>(0))));
    const auto center = glm::vec<L, T>(t * glm::vec<L+1, T>(b.center(), static_cast<T>(1)));

    return BoundingBoxT<L, T>({ center - halfSize, center + halfSize });
}

template <glm::length_t L, typename T>
inline typename BoundingBoxT<L, T>::PointType BoundingBoxT<L, T>::point(size_t index) const {
    assert(index < numPoints());
    std::bitset<L> bits(index);
    PointType result;
    for (glm::length_t i = 0u; i < L; ++i)
        result[i] = m_points[bits[i]][i];
    return result;
}

template<glm::length_t L, typename T>
inline std::vector<typename BoundingBoxT<L, T>::PointType> BoundingBoxT<L, T>::points() const
{
    std::vector<PointType> result;
    if (!isEmpty())
    {
        result.reserve(numPoints());
        for (size_t p = 0u; p < numPoints(); ++p)
            result.push_back(point(p));
    }
    return result;
}

template<glm::length_t L, typename T>
inline std::array<size_t, 2u> BoundingBoxT<L, T>::edgeIndices(size_t index)
{
    assert(index < (BoundingBoxT<L, T>::numEdges()));
    return { std::bitset<L>(NthNumberWithEvenBitsCount(index / L)).to_ullong(),
             std::bitset<L>(NthNumberWithEvenBitsCount(index / L)).flip(index % L).to_ullong() };
}

template<glm::length_t L, typename T>
inline typename BoundingBoxT<L, T>::EdgeType BoundingBoxT<L, T>::edge(size_t index) const
{
    auto indices = edgeIndices(index);
    return EdgeType({ point(indices[0u]), point(indices[1u]) });
}

template<glm::length_t L, typename T>
inline std::vector<typename BoundingBoxT<L, T>::EdgeType> BoundingBoxT<L, T>::edges() const
{
    std::vector<EdgeType> result;
    if (!isEmpty())
    {
        result.reserve(numEdges());
        for (size_t e = 0u; e < numEdges(); ++e)
            result.push_back(edge(e));
    }
    return result;
}

template <glm::length_t L, typename T>
inline bool BoundingBoxT<L, T>::isEmpty() const {
    for (glm::length_t k = 0; k < L; ++k)
        if (minPoint()[k] > maxPoint()[k]) return true;
    return false;
}

template <glm::length_t L, typename T>
inline int BoundingBoxT<L, T>::classifyPoint(const PointType &p) const
{
    int result = +1;
    for (size_t i = 0u; i < length(); ++i) {
        if ((p[i] < minPoint()[i]) || (p[i] > maxPoint()[i])) {
            result = -1;
            break;
        }
    }
    return result;
}

template<glm::length_t L, typename T>
inline typename BoundingBoxT<L, T>::PointType BoundingBoxT<L, T>::closestPoint(const PointType &v) const
{
    return glm::clamp(v, minPoint(), maxPoint());
}

template <glm::length_t L, typename T>
inline RangeT<T> BoundingBoxT<L, T>::projectOnLine(const LineT<L, T> &l) const
{
    if (isEmpty())
        return RangeT<T>();

    const auto &orig = l.origin();
    const auto &dir = l.direction();
    PointType vmin = minPoint(), vmax = maxPoint();
    for (glm::length_t k = 0; k < L; ++k)
        if (dir[k] < static_cast<T>(0)) std::swap(vmin[k], vmax[k]);
    return { glm::dot(vmin - orig, dir), glm::dot(vmax - orig, dir) };
}

template <glm::length_t L, typename T>
inline RangeT<T> BoundingBoxT<L, T>::pairDistancesToPlane(const PlaneT<L, T> &p) const
{
    return projectOnLine(LineT<L, T>(p.anyPoint(), p.normal()));
}

template<glm::length_t L, typename T>
inline T BoundingBoxT<L, T>::distanceToPlane(const PlaneT<L, T> &p) const
{
    const auto dists = pairDistancesToPlane(p);
    return (dists.nearValue() * dists.farValue() <= static_cast<T>(0)) ?
                static_cast<T>(0) :
                ((dists.nearValue() > static_cast<T>(0)) ? dists.nearValue() : dists.farValue());
}

template<glm::length_t L, typename T>
inline int BoundingBoxT<L, T>::classifyByPlane(const PlaneT<L, T> &p) const
{
    const auto dists = pairDistancesToPlane(p);
    return (dists.nearValue() * dists.farValue() <= static_cast<T>(0)) ?
                0:
                ((dists.nearValue() > static_cast<T>(0)) ? +1 : -1);
}

template <glm::length_t L, typename T>
inline BoundingBoxT<L, T> &BoundingBoxT<L, T>::operator +=(const BoundingBoxT<L, T> &b) { *this = *this + b; return *this; }

template <glm::length_t L, typename T>
inline BoundingBoxT<L, T> &BoundingBoxT<L, T>::operator +=(const typename BoundingBoxT<L, T>::PointType &v) { *this = *this + v; return *this; }

template <glm::length_t L, typename T>
inline BoundingBoxT<L, T> &BoundingBoxT<L, T>::operator *=(const BoundingBoxT<L, T> &b) { *this = *this * b; return *this; }

} // namespace
} // namespace

#endif // UTILS_BOUNDINGBOX_H

#ifndef UTILS_BOUNDINGBOX_H
#define UTILS_BOUNDINGBOX_H

#include <array>

#include <utils/glm/detail/type_vec1.hpp>
#include <utils/glm/detail/type_vec2.hpp>
#include <utils/glm/detail/type_vec3.hpp>
#include <utils/forwarddecl.h>
#include <utils/range.h>
#include <utils/transform.h>
#include <utils/plane.h>

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
    static constexpr glm::length_t length() { return L; }
    using value_type = T;
    using PointType = glm::vec<L, T>;

    BoundingBoxT() { *this = empty(); }
    BoundingBoxT(const PointType &minP, const PointType &maxP) : m_minPoint(minP), m_maxPoint(maxP) {}

    bool isEmpty() const {
        for (glm::length_t k = 0; k < L; ++k)
            if (m_minPoint[k] > m_maxPoint[k]) return true;
        return false;
    }

    RangeT<T> pairDistancesToPlane(const PlaneT<L, T> &p) const
    {
        PointType vmin = m_minPoint, vmax = m_maxPoint;
        const auto normal = p.normal();
        for (glm::length_t k = 0; k < L; ++k)
            if (normal[k] < static_cast<T>(0)) std::swap(vmin[k], vmax[k]);
        const T vMinDist = p.distanceTo(vmin), vMaxDist = p.distanceTo(vmax);
        return { vMinDist, vMaxDist };
    }

    T distanceToPlane(const PlaneT<L, T> &p) const {
        const auto dists = pairDistancesToPlane(p);
        return (dists.nearValue() * dists.farValue() <= static_cast<T>(0)) ?
                    static_cast<T>(0) :
                    ((dists.nearValue() > static_cast<T>(0)) ? dists.nearValue() : dists.farValue());
    }

    PointType closestPoint(const PointType &v) const
    {
        return glm::clamp(v, m_minPoint, m_maxPoint);
    }

    const PointType &minPoint() const { return m_minPoint; }
    const PointType &maxPoint() const { return m_maxPoint; }
    PointType center() const { return static_cast<T>(.5) * (m_minPoint + m_maxPoint); }
    PointType halfSize() const { return static_cast<T>(.5) * (m_maxPoint - m_minPoint); }

    BoundingBoxT<L, T> &operator += (const BoundingBoxT<L, T> &b);
    BoundingBoxT<L, T> &operator += (const PointType &v);

    static BoundingBoxT<L, T> fromMinMax(const PointType &minP, const PointType &maxP) { return BoundingBoxT<L, T>(minP, maxP); }
    static BoundingBoxT<L, T> fromCenterHalfSize(const PointType &cP, const PointType &hs) { return BoundingBoxT<L, T>(cP-hs, cP+hs); }
    static BoundingBoxT<L, T> empty() { return BoundingBoxT<L, T>(PointType(std::numeric_limits<T>::max()),
                                                                  PointType(std::numeric_limits<T>::lowest())); }

private:
    PointType m_minPoint, m_maxPoint;
};

template <glm::length_t L, typename T>
inline BoundingBoxT<L, T> operator +(const BoundingBoxT<L, T> &b1, const BoundingBoxT<L, T> &b2)
{
    return BoundingBoxT<L, T>(glm::min(b1.minPoint(), b2.minPoint()), glm::max(b1.maxPoint(), b2.maxPoint()));
}

template <glm::length_t L, typename T>
inline BoundingBoxT<L, T> operator +(const BoundingBoxT<L, T> &b, const typename BoundingBoxT<L, T>::PointType &v)
{
    return BoundingBoxT<L, T>(glm::min(b.minPoint(), v), glm::max(b.maxPoint(), v));
}

template <glm::length_t L, typename T>
inline BoundingBoxT<L, T> operator +(const typename BoundingBoxT<L, T>::PointType &v, const BoundingBoxT<L, T> &b)
{
    return b + v;
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

    return BoundingBoxT<L, T>(center - halfSize, center + halfSize);
}

template <glm::length_t L, typename T>
inline BoundingBoxT<L, T> operator *(const glm::mat<L+1, L+1, T> &t, const BoundingBoxT<L, T> &b)
{
    if (b.isEmpty()) return BoundingBoxT<L, T>::empty();

    const auto halfSize = glm::abs(glm::vec<L, T>(t * glm::vec<L+1, T>(b.halfSize(), static_cast<T>(0))));
    const auto center = glm::vec<L, T>(t * glm::vec<L+1, T>(b.center(), static_cast<T>(1)));

    return BoundingBoxT<L, T>(center - halfSize, center + halfSize);
}

template <glm::length_t L, typename T>
inline BoundingBoxT<L, T> &BoundingBoxT<L, T>::operator +=(const BoundingBoxT<L, T> &b) { *this = *this + b; return *this; }

template <glm::length_t L, typename T>
inline BoundingBoxT<L, T> &BoundingBoxT<L, T>::operator +=(const typename BoundingBoxT<L, T>::PointType &v) { *this = *this + v; return *this; }

} // namespace
} // namespace

#endif // UTILS_BOUNDINGBOX_H

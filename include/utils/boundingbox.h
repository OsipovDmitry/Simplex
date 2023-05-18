#ifndef UTILS_BOUNDINGBOX_H
#define UTILS_BOUNDINGBOX_H

#include <array>

#include <utils/glm/detail/type_vec1.hpp>
#include <utils/glm/detail/type_vec2.hpp>
#include <utils/glm/detail/type_vec3.hpp>
#include <utils/forwarddecl.h>
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

    BoundingBoxT() : m_minPoint(std::numeric_limits<float>::max()), m_maxPoint(-std::numeric_limits<float>::max()) {}
    BoundingBoxT(const PointType &minP, const PointType &maxP) : m_minPoint(minP), m_maxPoint(maxP) {}

    bool isEmpty() const {
        for (glm::length_t k = 0; k < L; ++k)
            if (m_minPoint[k] > m_maxPoint[k]) return true;
        return false;
    }

    float distanceToPlane(const PlaneT<L, T> &p) const {
        PointType vmin = m_minPoint, vmax = m_maxPoint;
        const auto normal = p.normal();
        for (glm::length_t k = 0; k < L; ++k)
            if (normal[k] < .0f) std::swap(vmin[k], vmax[k]);
        const float vMinDist = p.distanceTo(vmin), vMaxDist = p.distanceTo(vmax);
        return (vMinDist * vMaxDist <= .0f) ?
                    .0f :
                    (vMinDist > .0f) ? vMinDist  : vMaxDist;
    }

    std::array<float, 2> pairDistancesToPlane(const PlaneT<L, T> &p) const
    {
        PointType vmin = m_minPoint, vmax = m_maxPoint;
        const auto normal = p.normal();
        for (glm::length_t k = 0; k < L; ++k)
            if (normal[k] < .0f) std::swap(vmin[k], vmax[k]);
        const float vMinDist = p.distanceTo(vmin), vMaxDist = p.distanceTo(vmax);
        return { vMinDist, vMaxDist };
    }

    PointType closestPoint(const PointType &v) const
    {
        return glm::clamp(v, m_minPoint, m_maxPoint);
    }

    const PointType &minPoint() const { return m_minPoint; }
    const PointType &maxPoint() const { return m_maxPoint; }
    PointType center() const { return .5f * (m_minPoint + m_maxPoint); }
    PointType halfSize() const { return .5f * (m_maxPoint - m_minPoint); }

    BoundingBoxT<L, T> &operator += (const BoundingBoxT<L, T> &b);
    BoundingBoxT<L, T> &operator += (const PointType &v);

    static BoundingBoxT<L, T> fromMinMax(const PointType &minP, const PointType &maxP) { return BoundingBoxT<L, T>(minP, maxP); }
    static BoundingBoxT<L, T> fromCenterHalfSize(const PointType &cP, const PointType &hz) { return BoundingBoxT<L, T>(cP-hz, cP+hz); }

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
    if (b.isEmpty()) return BoundingBoxT<L, T>();

    const auto xAxis = glm::abs(t.rotation * glm::vec3(1.f, 0.f, 0.f));
    const auto yAxis = glm::abs(t.rotation * glm::vec3(0.f, 1.f, 0.f));
    const auto zAxis = glm::abs(t.rotation * glm::vec3(0.f, 0.f, 1.f));

    const auto halfSize = glm::mat<L, L, T>(glm::mat3(xAxis, yAxis, zAxis)) * (b.halfSize() * t.scale);
    const auto center = t * b.center();

    return BoundingBoxT<L, T>(center - halfSize, center + halfSize);
}

template <glm::length_t L, typename T>
inline BoundingBoxT<L, T> &BoundingBoxT<L, T>::operator +=(const BoundingBoxT<L, T> &b) { *this = *this + b; return *this; }

template <glm::length_t L, typename T>
inline BoundingBoxT<L, T> &BoundingBoxT<L, T>::operator +=(const typename BoundingBoxT<L, T>::PointType &v) { *this = *this + v; return *this; }

} // namespace
} // namespace

#endif // UTILS_BOUNDINGBOX_H

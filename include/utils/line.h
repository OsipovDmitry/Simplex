#ifndef UTILS_LINE_H
#define UTILS_LINE_H

#include <utils/glm/detail/type_vec1.hpp>
#include <utils/glm/detail/type_vec2.hpp>
#include <utils/glm/detail/type_vec3.hpp>
#include <utils/forwarddecl.h>
#include <utils/epsilon.h>
#include <utils/transform.h>
#include <utils/plane.h>

namespace simplex
{
namespace utils
{

template <glm::length_t L, typename T>
struct LineT
{
    static_assert((L >= 1) && (L < 4), "The dimaension of Line must be [1..3]");
    static_assert(std::numeric_limits<T>::is_iec559, "The base type of Line must be floating point");

public:
    static constexpr glm::length_t length() { return L; }
    using value_type = T;
    using PointType = glm::vec<L, T>;

    LineT(const PointType &orig, const PointType &dir) : m_origin(orig), m_direction(dir) { m_direction = glm::normalize(m_direction); }

    const PointType &origin() const { return m_origin; }
    const PointType &direction() const { return m_direction; }

    PointType point(T t) const { return m_origin + t * m_direction; }

    bool intersectPlane(const PlaneT<L, T> &p, T &tRes) const;

    T projectOn(const PointType& v) const { return glm::dot(m_direction, v - m_origin); }

private:
    PointType m_origin;
    PointType m_direction;
};

template <glm::length_t L, typename T>
inline bool LineT<L, T>::intersectPlane(const PlaneT<L, T> &p, T &tRes) const
{
    const auto denom = glm::dot(p.normal(), m_direction);
    if (glm::abs(denom) < utils::epsilon<T>()) return false;
    tRes = -p.distanceTo(m_origin) / denom;
    return true;
}

template <glm::length_t L, typename T>
inline LineT<L, T> operator *(const TransformT<L, T> &t, const LineT<L, T> &l)
{
    const auto point0 = t * l.point(static_cast<T>(0));
    const auto point1 = t * l.point(static_cast<T>(1));

    return LineT<L, T>(t * l.origin(), point1 - point0);
}

} // namespace
} // namespace

#endif // UTILS_LINE_H

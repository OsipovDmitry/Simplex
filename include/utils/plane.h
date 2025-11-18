#ifndef UTILS_PLANE_H
#define UTILS_PLANE_H

#include <utils/glm/detail/type_vec1.hpp>
#include <utils/glm/detail/type_vec2.hpp>
#include <utils/glm/detail/type_vec3.hpp>
#include <utils/glm/detail/type_vec4.hpp>
#include <utils/glm/geometric.hpp>
#include <utils/forwarddecl.h>
#include <utils/transform.h>

namespace simplex
{
namespace utils
{

template<glm::length_t L, typename T>
struct PlaneT
{
    static_assert((L >= 1) && (L < 4), "The dimaension of Plane must be [1..3]");
    static_assert(std::numeric_limits<T>::is_iec559, "The base type of Plane must be floating point");

public:
    static constexpr glm::length_t length() { return L; }
    using value_type = T;
    using PointType = glm::vec<L, T>;
    using BaseType = glm::vec<L + 1, T>;

    PlaneT(const BaseType &v = BaseType(glm::vec4(1.f, 0.f, 0.f, 0.f))) : m_coefs(v) { m_coefs /= glm::length(normal()); }
    PlaneT(const PointType &n, T d) : m_coefs(glm::normalize(n), -d) {}

    operator BaseType() const { return m_coefs; };

    PointType normal() const { return PointType(m_coefs); }
    void setNormal(const PointType &n) { m_coefs = BaseType(glm::normalize(n), dist()); }

    value_type dist() const { return -m_coefs[L]; }
    void setDist(value_type d) { m_coefs[L] = -d; }

    PointType anyPoint(value_type distanceToPlane = 0.0) const { return normal() * (distanceToPlane + dist()); }

    value_type distanceTo(const PointType &v) const { return glm::dot(m_coefs, BaseType(v, static_cast<value_type>(1))); }

private:
    BaseType m_coefs;
};

template<glm::length_t L, typename T>
inline PlaneT<L, T> operator *(const TransformT<L, T> &t, const PlaneT<L, T> &p)
{
    const auto anyPoint0 = t * p.anyPoint(static_cast<T>(0));
    const auto anyPoint1 = t * p.anyPoint(static_cast<T>(1));

    const auto newNormal = glm::normalize(anyPoint1 - anyPoint0);
    return PlaneT<L, T>(newNormal, glm::dot(newNormal, anyPoint0));
}

} // namespace
} // namespace

#endif // UTILS_PLANE_H

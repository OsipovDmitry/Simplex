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
struct PlaneT : protected glm::vec<L+1, T>
{
    static_assert((L >= 1) && (L < 4), "The dimaension of Plane must be [1..3]");
    static_assert(std::numeric_limits<T>::is_iec559, "The base type of Plane must be floating point");

public:
    static constexpr glm::length_t length() { return L; }
    using value_type = T;

    using PointType = glm::vec<L, T>;
    using BaseType = glm::vec<L+1, T>;

    PlaneT(const BaseType &v = BaseType(glm::vec4(1.f, 0.f, 0.f, 0.f))) : glm::vec<L+1, T>(v) { *this /= glm::length(normal()); }
    PlaneT(const PointType &n, T d) : glm::vec<L+1, T>(glm::normalize(n), -d) {}

    PointType normal() const { return PointType(*this); }
    void setNormal(const PointType &n) { *this = PlaneT<L, T>(glm::normalize(n), dist()); }

    T dist() const { return -this->operator [](L); }
    void setDist(T d) { this->operator [](L) = -d; }

    PointType anyPoint(T distanceToPlane = 0.0) const { return normal() * (distanceToPlane + dist()); }

    T distanceTo(const PointType &v) const { return glm::dot(*this, BaseType(v, 1.0)); }
};

template<glm::length_t L, typename T>
inline PlaneT<L, T> operator *(const TransformT<L, T> &t, const PlaneT<L, T> &p)
{
    const auto anyPoint0 = t * p.anyPoint(0.0);
    const auto anyPoint1 = t * p.anyPoint(1.0);

    const auto newNormal = glm::normalize(anyPoint1 - anyPoint0);
    return PlaneT<L, T>(newNormal, glm::dot(newNormal, anyPoint0));
}

} // namespace
} // namespace

#endif // UTILS_PLANE_H

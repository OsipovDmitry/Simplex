#ifndef UTILS_LINESEGMENT_H
#define UTILS_LINESEGMENT_H

#include <array>

#include <utils/glm/detail/type_vec1.hpp>
#include <utils/glm/detail/type_vec2.hpp>
#include <utils/glm/detail/type_vec3.hpp>
#include <utils/forwarddecl.h>
#include <utils/epsilon.h>
#include <utils/transform.h>
#include <utils/plane.h>
#include <utils/line.h>

namespace simplex
{
namespace utils
{

template <glm::length_t L, typename T>
struct LineSegmentT
{
    static_assert((L >= 1) && (L < 4), "The dimaension of LineSegment must be [1..3]");
    static_assert(std::numeric_limits<T>::is_iec559, "The base type of LineSegment must be floating point");

public:
    static constexpr glm::length_t length() { return L; }
    using value_type = T;
    using PointType = glm::vec<L, T>;

    LineSegmentT(const std::array<PointType, 2u> &pp) : m_points(pp) {}

    const std::array<PointType, 2u> &points() const { return m_points; }
    std::array<PointType, 2u> &points() { return m_points; }

    const PointType &startPoint() const { return m_points[0u]; }
    void setStartPoint(const PointType &value) { m_points[0u] = value; }

    const PointType &endPoint() const { return m_points[1u]; }
    void setEndPoint(const PointType &value) { m_points[1u] = value; }
    
    int classifyByPlane(const PlaneT<L, T> &p) const; // 0 - intersect, +1 - in front, -1 - in back
    bool intersectPlane(const PlaneT<L, T> &p, PointType &resPoint) const;

private:
    std::array<PointType, 2u> m_points;
};

template <glm::length_t L, typename T>
int LineSegmentT<L, T>::classifyByPlane(const PlaneT<L, T> &p) const
{
    const auto s1 = p.distanceTo(m_points[0u]), s2 = p.distanceTo(m_points[1u]);
    if (s1 > 0.f && s2 > 0.f) return +1;
    else if (s1 < 0.f && s2 < 0.f) return -1;
    else return 0;
}

template <glm::length_t L, typename T>
bool LineSegmentT<L, T>::intersectPlane(const PlaneT<L, T> &p, PointType &resPoint) const
{
    const auto dir = endPoint() - startPoint();
    const auto dirLen = glm::length(dir);
    if (dirLen < utils::epsilon<T>()) return false;
    LineT<L, T> l(startPoint(), dir);
    T t;
    if (!l.intersectPlane(p, t)) return false;
    if ((t < static_cast<T>(0)) || (t > dirLen)) return false;
    resPoint = l.point(t);
    return true;
}

template <glm::length_t L, typename T>
inline LineSegmentT<L, T> operator *(const TransformT<L, T> &t, const LineSegmentT<L, T> &ls)
{
    return LineSegmentT<L, T>({ t * ls.startPoint(), t * ls.endPoint() });
}

} // namespace
} // namespace

#endif // UTILS_LINESEGMENT_H

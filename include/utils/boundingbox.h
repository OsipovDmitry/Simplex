#ifndef UTILS_BOUNDINGBOX_H
#define UTILS_BOUNDINGBOX_H

#include <vector>

#include <utils/glm/vec3.hpp>
#include <utils/transform.h>
#include <utils/plane.h>

namespace simplex
{
namespace utils
{

struct BoundingBox;
inline BoundingBox operator +(const BoundingBox&, const BoundingBox&);

struct BoundingBox
{
    glm::vec3 minPoint, maxPoint;

    BoundingBox() : minPoint(std::numeric_limits<float>::max()), maxPoint(-std::numeric_limits<float>::max()) {}
    BoundingBox(const glm::vec3 &minP, const glm::vec3 &maxP) : minPoint(minP), maxPoint(maxP) {}

    bool isEmpty() const {
        for (glm::vec3::length_type k = 0; k < 3; ++k)
            if (minPoint[k] > maxPoint[k]) return true;
        return false;
    }

    float distanceToPlane(const Plane &p) const {
        glm::vec3 vmin = minPoint, vmax = maxPoint;
        for (glm::vec3::length_type k = 0; k < 3; ++k)
            if (p[k] < .0f) std::swap(vmin[k], vmax[k]);
        const float vMinDist = p.distanceTo(vmin), vMaxDist = p.distanceTo(vmax);
        return (vMinDist * vMaxDist <= .0f) ?
                    .0f :
                    (vMinDist > .0f) ? vMinDist  : vMaxDist;
    }

    std::pair<float, float> pairDistancesToPlane(const Plane &p) const
    {
        glm::vec3 vmin = minPoint, vmax = maxPoint;
        for (glm::vec3::length_type k = 0; k < 3; ++k)
            if (p[k] < .0f) std::swap(vmin[k], vmax[k]);
        const float vMinDist = p.distanceTo(vmin), vMaxDist = p.distanceTo(vmax);
        return std::make_pair(vMinDist, vMaxDist);
    }

    glm::vec3 closestPoint(const glm::vec3 &v) const
    {
        return glm::clamp(v, minPoint, maxPoint);
    }

    glm::vec3 center() const { return .5f * (minPoint + maxPoint); }
    glm::vec3 halfSize() const { return .5f * (maxPoint - minPoint); }

    BoundingBox &operator += (const BoundingBox &b) { *this = *this + b; return *this; }

    static BoundingBox fromMinMax(const glm::vec3 &minP, const glm::vec3 &maxP) { return BoundingBox(minP, maxP); }
    static BoundingBox fromCenterHalfSize(const glm::vec3 &cP, const glm::vec3 &hz) { return BoundingBox(cP-hz, cP+hz); }
};

inline BoundingBox operator +(const BoundingBox &b1, const BoundingBox &b2)
{
    BoundingBox result;
    for (glm::vec3::length_type k = 0; k < 3; ++k) {
        result.minPoint[k] = (b1.minPoint[k] < b2.minPoint[k]) ? b1.minPoint[k] : b2.minPoint[k];
        result.maxPoint[k] = (b1.maxPoint[k] > b2.maxPoint[k]) ? b1.maxPoint[k] : b2.maxPoint[k];
    }
    return result;
}

inline BoundingBox operator *(const Transform &t, const BoundingBox& b)
{
    if (b.isEmpty()) return BoundingBox();

    const auto xAxis = glm::abs(glm::vec3(t.rotation * glm::vec4(1.f, 0.f, 0.f, 1.f)));
    const auto yAxis = glm::abs(glm::vec3(t.rotation * glm::vec4(0.f, 1.f, 0.f, 1.f)));
    const auto zAxis = glm::abs(glm::vec3(t.rotation * glm::vec4(0.f, 0.f, 1.f, 1.f)));

    const glm::vec3 halfSize = glm::mat3x3(xAxis, yAxis, zAxis) * (b.halfSize() * t.scale);
    const glm::vec3 center = t * b.center();

    return BoundingBox(center - halfSize, center + halfSize);
}

} // namespace
} // namespace

#endif // UTILS_BOUNDINGBOX_H

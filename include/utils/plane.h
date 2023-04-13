#ifndef UTILS_PLANE_H
#define UTILS_PLANE_H

#include <utils/glm/vec3.hpp>
#include <utils/glm/vec4.hpp>
#include <utils/glm/geometric.hpp>

namespace simplex
{
namespace utils
{

struct Plane : public glm::vec4
{
public:
    Plane() : glm::vec4(0.f, 1.f, 0.f, 0.f) {}
    Plane(const glm::vec3 &n, float d) : glm::vec4(glm::normalize(n), -d) {}
    Plane(const glm::vec4 &v) : glm::vec4(v) { *this /= glm::length(normal()); }

    glm::vec3 normal() const { return glm::vec3(x, y, z); }
    void setNormal(const glm::vec3 &n) { glm::vec3 nn = glm::normalize(n); x = nn.x; y = nn.y; z = nn.z; }

    float dist() const { return -w; }
    void setDist(float d) { w = -d; }

    float distanceTo(const glm::vec3 &v) const { return glm::dot(*this, glm::vec4(v, 1.f)); }

};

} // namespace
} // namespace

#endif // UTILS_PLANE_H

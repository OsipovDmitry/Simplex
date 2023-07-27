#ifndef UTILS_SCREENQUADDATA_H
#define UTILS_SCREENQUADDATA_H

#include <vector>

#include <utils/glm/vec4.hpp>

namespace simplex
{
namespace utils
{

static const std::vector<glm::vec4> s_screenQuadVertices {
    {-1.f, +1.f, .0f, 1.f},
    {-1.f, -1.f, .0f, 1.f},
    {+1.f, +1.f, .0f, 1.f},
    {+1.f, -1.f, .0f, 1.f}
};

}
}

#endif // UTILS_SCREENQUADDATA_H

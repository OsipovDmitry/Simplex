#ifndef UTILS_TRIANGLEDATA_H
#define UTILS_TRIANGLEDATA_H

#include <vector>

#include <utils/glm/gtc/constants.hpp>
#include <utils/glm/vec4.hpp>

namespace simplex
{
namespace utils
{

static const std::vector<glm::vec4> s_triangleVertices {
    {-.5f, -1.f/(2.f*glm::root_three<float>()), .0f, 1.f},
    {+.5f, -1.f/(2.f*glm::root_three<float>()), .0f, 1.f},
    {+.0f, +1.f/(    glm::root_three<float>()), .0f, 1.f},
};

static const std::vector<glm::vec4> s_triangleNormals {
    {0.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f, 0.0f},
};

static const std::vector<glm::vec4> s_triangleTexCoords {
    {0.0f, 0.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 0.0f, 1.0f},
    {0.5f, 1.0f, 0.0f, 1.0f},
};

static const std::vector<uint32_t> s_triangleIndices {
    0u, 1u, 2u
};


}
}

#endif // UTILS_TRIANGLEDATA_H

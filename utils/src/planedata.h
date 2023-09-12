#ifndef UTILS_PLANEDATA_H
#define UTILS_PLANEDATA_H

#include <vector>

#include <utils/glm/vec4.hpp>

namespace simplex
{
namespace utils
{

static const std::vector<glm::vec4> s_planeVertices {
    {-0.500000f, -0.500000f, 0.000000f, 1.000000f},
    {+0.500000f, -0.500000f, 0.000000f, 1.000000f},
    {+0.500000f, +0.500000f, 0.000000f, 1.000000f},
    {-0.500000f, +0.500000f, 0.000000f, 1.000000f},
};

static const std::vector<glm::vec4> s_planeNormals {
    {0.000000f, 0.000000f, 1.000000f, 0.000000f},
    {0.000000f, 0.000000f, 1.000000f, 0.000000f},
    {0.000000f, 0.000000f, 1.000000f, 0.000000f},
    {0.000000f, 0.000000f, 1.000000f, 0.000000f},
};

static const std::vector<glm::vec4> s_planeTexCoords {
    {0.000000f, 0.000000f, 0.000000f, 1.000000f},
    {1.000000f, 0.000000f, 0.000000f, 1.000000f},
    {1.000000f, 1.000000f, 0.000000f, 1.000000f},
    {0.000000f, 1.000000f, 0.000000f, 1.000000f},
};

static const std::vector<uint32_t> s_planeIndices {
    0u, 1u, 2u,
    0u, 2u, 3u,
};

}
}

#endif // UTILS_PLANEDATA_H

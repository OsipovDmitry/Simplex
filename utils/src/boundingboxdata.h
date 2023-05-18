#ifndef UTILS_BOUNDINGBOXDATA_H
#define UTILS_BOUNDINGBOXDATA_H

#include <vector>

#include <utils/glm/vec4.hpp>

namespace simplex
{
namespace utils
{

static const std::vector<glm::vec4> s_boundingBoxVertices {
    {-1.000000f, -1.000000f, -1.000000f, 1.000000f},
    {-1.000000f, +1.000000f, -1.000000f, 1.000000f},
    {+1.000000f, +1.000000f, -1.000000f, 1.000000f},
    {+1.000000f, -1.000000f, -1.000000f, 1.000000f},
    {-1.000000f, -1.000000f, +1.000000f, 1.000000f},
    {-1.000000f, +1.000000f, +1.000000f, 1.000000f},
    {+1.000000f, +1.000000f, +1.000000f, 1.000000f},
    {+1.000000f, -1.000000f, +1.000000f, 1.000000f},
};

static const std::vector<uint32_t> s_boundingBoxIndices {
    0u, 1u,
    1u, 2u,
    2u, 3u,
    3u, 0u,
    4u, 5u,
    5u, 6u,
    6u, 7u,
    7u, 4u,
    0u, 4u,
    1u, 5u,
    2u, 6u,
    3u, 7u,
};

}
}

#endif // UTILS_BOUNDINGBOXDATA_H

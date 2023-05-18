#ifndef UTILS_VECCAST_H
#define UTILS_VECCAST_H

#include <utils/glm/detail/qualifier.hpp>
#include <utils/glm/common.hpp>

namespace simplex
{
namespace utils
{

template <glm::length_t L1, glm::length_t L2, typename T>
inline glm::vec<L1, T> vec_cast(const glm::vec<L2, T> &v, T defaultValue = static_cast<T>(0.0)) {
    glm::vec<L1, T> result(defaultValue);
    for (glm::length_t i = 0; i < glm::min(L1, L2); ++i)
        result[i] = v[i];
    return result;
}

}
}

#endif // UTILS_VECCAST_H

#ifndef UTILS_EPSILON_H
#define UTILS_EPSILON_H

namespace simplex
{
namespace utils
{

template <typename T>
constexpr T epsilon()
{
    return static_cast<T>(1e-3);
}

template <typename T>
constexpr T maximum()
{
    return static_cast<T>(1234567.f/*1e+6*/);
}

}
}

#endif // UTILS_EPSILON_H

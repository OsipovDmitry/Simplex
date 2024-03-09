#ifndef UTILS_RANGE_H
#define UTILS_RANGE_H

#include <limits>
#include <array>

namespace simplex
{
namespace utils
{

template <typename T>
struct RangeT : public std::array<T, 2u>
{
public:
    using value_type = T;

    RangeT(const T &n = std::numeric_limits<T>::max(), const T &f = std::numeric_limits<T>::lowest())
        : std::array<T, 2u>({n, f})
    {
    }

    bool isEmpty() const { return this->operator [](0u) >= this->operator [](1u); }

    const T &nearValue() const { return this->operator [](0u); }
    void setNearValue(const T &n) { this->operator [](0u) = n; }

    const T &farValue() const { return this->operator [](1u);  }
    void setFarValue(const T &f) { this->operator [](1u) = f; }
};

} // namespace
} // namespace


#endif // UTILS_RANGE_H

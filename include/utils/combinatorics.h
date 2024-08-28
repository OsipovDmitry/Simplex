#ifndef UTILS_COMBINATORICS_H
#define UTILS_COMBINATORICS_H

#include <vector>

namespace simplex
{
namespace utils
{

template <typename T>
constexpr T factorial(T n)
{
    return n ? n * factorial(n-1) : 1;
}

template <typename T>
constexpr T pow2(T n)
{
    return 1 << n;
}

template <typename T>
constexpr T numberOfKDimFacesOfNDimBox(T k, T n)
{
    return pow2(n-k) * factorial(n) / factorial(k) / factorial(n-k);
}

template <typename T>
inline T NthNumberWithEvenBitsCount(T n)
{
    static const std::vector<T> s_table {
        0, 3, 5, 6, 9, 10, 12, 15, 17, 18,
        20, 23, 24, 27, 29, 30, 33, 34, 36,
        39, 40, 43, 45, 46, 48, 51, 53, 54,
        57, 58, 60, 63, 65, 66, 68, 71, 72,
        75, 77, 78, 80, 83, 85, 86, 89, 90,
        92, 95, 96, 99, 101, 102, 105, 106, 108,
        111, 113, 114, 116, 119, 120, 123, 125, 126
    };

    assert(n < 64); // s_table has been calculated for the first 64 numbers; If it's required more, add values to the s_table
    return s_table[n];
}

}
}

#endif // UTILS_COMBINATORICS_H

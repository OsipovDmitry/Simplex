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

    bool isEmpty() const { return this->operator [](0u) > this->operator [](1u); }

    const T &nearValue() const { return this->operator [](0u); }
    void setNearValue(const T &n) { this->operator [](0u) = n; }

    const T &farValue() const { return this->operator [](1u);  }
    void setFarValue(const T &f) { this->operator [](1u) = f; }

    RangeT<T> &operator +=(float value);
    RangeT<T> &operator -=(float value);
};

template <typename T>
inline RangeT<T> operator +(const RangeT<T> &range, float value) {
    return RangeT<T>(range.nearValue() + value, range.farValue() + value);
}

template <typename T>
inline RangeT<T> operator +(float value, const RangeT<T> &range) { return range + value; }

template <typename T>
inline RangeT<T> operator -(const RangeT<T> &range, float value) { return range + (-value); }

template<typename T>
inline RangeT<T> &RangeT<T>::operator +=(float value) { *this = *this + value; return *this; }

template<typename T>
inline RangeT<T> &RangeT<T>::operator -=(float value) { *this = *this - value; return *this; }

} // namespace
} // namespace


#endif // UTILS_RANGE_H

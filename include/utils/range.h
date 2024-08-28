#ifndef UTILS_RANGE_H
#define UTILS_RANGE_H

#include <utils/glm/vec2.hpp>
#include <utils/glm/common.hpp>
#include <utils/epsilon.h>

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

    RangeT(const T &n = utils::maximum<T>(), const T &f = -utils::maximum<T>())
        : std::array<T, 2u>({n, f})
    {
    }

    RangeT(const glm::vec<2u, T> &v)
        : RangeT(v.x, v.y)
    {}

    bool isEmpty() const { return this->operator [](0u) > this->operator [](1u); }

    const T &nearValue() const { return this->operator [](0u); }
    void setNearValue(const T &n) { this->operator [](0u) = n; }

    const T &farValue() const { return this->operator [](1u);  }
    void setFarValue(const T &f) { this->operator [](1u) = f; }

    RangeT<T> &scale(const T &value);
    RangeT<T> scaled(const T &value) const;

    RangeT<T> &translate(const T &value);
    RangeT<T> translated(const T &value) const;

    RangeT<T> &operator +=(const T &value);
    RangeT<T> &operator +=(const RangeT<T> &range);
    RangeT<T> &operator *=(const RangeT<T> &range);
};

template <typename T>
inline RangeT<T> operator +(const RangeT<T> &range, const T &value)
{
    return RangeT<T>(glm::min(range.nearValue(), value), glm::max(range.farValue(), value));
}

template <typename T>
inline RangeT<T> operator +(const T &value, const RangeT<T> &range)
{
    return range + value;
}

template <typename T>
inline RangeT<T> operator +(const RangeT<T> &range0, const RangeT<T> &range1)
{
    utils::RangeT<T> result;
    if (range0.isEmpty() && range1.isEmpty()) {}
    else if (range0.isEmpty()) { result = range1; }
    else if (range1.isEmpty()) { result = range0; }
    else { result = RangeT<T>(glm::min(range0.nearValue(), range1.nearValue()), glm::max(range0.farValue(), range1.farValue())); }
    return result;
}

template <typename T>
inline RangeT<T> operator *(const RangeT<T> &range0, const RangeT<T> &range1)
{
    return RangeT<T>(glm::max(range0.nearValue(), range1.nearValue()), glm::min(range0.farValue(), range1.farValue()));
}

template<typename T>
inline RangeT<T> &RangeT<T>::scale(const T &value)
{
    if (!isEmpty())
    {
        const auto valueAbs = glm::abs(value);
        setNearValue(nearValue() * valueAbs);
        setFarValue(farValue() * valueAbs);
    }
    return *this;
}

template<typename T>
inline RangeT<T> RangeT<T>::scaled(const T &value) const
{
    return RangeT<T>(*this).scaled(value);
}

template<typename T>
inline RangeT<T> &RangeT<T>::translate(const T &value)
{
    if (!isEmpty())
    {
        setNearValue(nearValue() + value);
        setFarValue(farValue() + value);
    }
    return *this;
}

template<typename T>
inline RangeT<T> RangeT<T>::translated(const T &value) const
{
    return RangeT<T>(*this).translate(value);
}

template<typename T>
inline RangeT<T> &RangeT<T>::operator +=(const T &value) { *this = *this + value; return *this; }

template<typename T>
inline RangeT<T> &RangeT<T>::operator +=(const RangeT<T> &range) { *this = *this + range; return *this; }

template<typename T>
inline RangeT<T> &RangeT<T>::operator *=(const RangeT<T> &range) { *this = *this * range; return *this; }

} // namespace
} // namespace


#endif // UTILS_RANGE_H

#ifndef UTILS_RANGE_H
#define UTILS_RANGE_H

#include <limits>

#include <utils/glm/common.hpp>
#include <utils/glm/vec2.hpp>

namespace simplex
{
namespace utils
{

template <typename T>
struct RangeT
{
public:
    using value_type = T;
    using BaseType = glm::vec<2u, T>;

    RangeT(const BaseType& v = BaseType(std::numeric_limits<value_type>::max(), std::numeric_limits<value_type>::lowest()))
        : m_range(v)
    {
    }

    RangeT(const value_type& n, const value_type& f)
        : RangeT(BaseType(n, f))
    {
    }

    operator BaseType() const { return m_range; };

    bool isEmpty() const { return m_range[0u] > m_range[1u]; }
    bool isInside(const T& value) const { return (value >= m_range[0u]) && (value <= m_range[1u]); }

    const T& nearValue() const { return m_range[0u]; }
    void setNearValue(const T& n) { m_range[0u] = n; }

    const T& farValue() const { return m_range[1u]; }
    void setFarValue(const T& f) { m_range[1u] = f; }

    RangeT<T>& scale(const T& value);
    RangeT<T> scaled(const T& value) const;

    RangeT<T>& translate(const T& value);
    RangeT<T> translated(const T& value) const;

    RangeT<T>& operator+=(const T& value);
    RangeT<T>& operator+=(const RangeT<T>& range);
    RangeT<T>& operator*=(const RangeT<T>& range);

private:
    BaseType m_range;
};

template <typename T>
inline RangeT<T> operator+(const RangeT<T>& range, const T& value)
{
    return RangeT<T>(glm::min(range.nearValue(), value), glm::max(range.farValue(), value));
}

template <typename T>
inline RangeT<T> operator+(const T& value, const RangeT<T>& range)
{
    return range + value;
}

template <typename T>
inline RangeT<T> operator+(const RangeT<T>& range0, const RangeT<T>& range1)
{
    utils::RangeT<T> result;
    if (range0.isEmpty() && range1.isEmpty())
    {
    }
    else if (range0.isEmpty())
    {
        result = range1;
    }
    else if (range1.isEmpty())
    {
        result = range0;
    }
    else
    {
        result = RangeT<T>(glm::min(range0.nearValue(), range1.nearValue()), glm::max(range0.farValue(), range1.farValue()));
    }
    return result;
}

template <typename T>
inline RangeT<T> operator*(const RangeT<T>& range0, const RangeT<T>& range1)
{
    return RangeT<T>(glm::max(range0.nearValue(), range1.nearValue()), glm::min(range0.farValue(), range1.farValue()));
}

template <typename T>
inline RangeT<T>& RangeT<T>::scale(const T& value)
{
    if (!isEmpty())
    {
        const auto valueAbs = glm::abs(value);
        setNearValue(nearValue() * valueAbs);
        setFarValue(farValue() * valueAbs);
    }
    return *this;
}

template <typename T>
inline RangeT<T> RangeT<T>::scaled(const T& value) const
{
    return RangeT<T>(*this).scaled(value);
}

template <typename T>
inline RangeT<T>& RangeT<T>::translate(const T& value)
{
    if (!isEmpty())
    {
        setNearValue(nearValue() + value);
        setFarValue(farValue() + value);
    }
    return *this;
}

template <typename T>
inline RangeT<T> RangeT<T>::translated(const T& value) const
{
    return RangeT<T>(*this).translate(value);
}

template <typename T>
inline RangeT<T>& RangeT<T>::operator+=(const T& value)
{
    *this = *this + value;
    return *this;
}

template <typename T>
inline RangeT<T>& RangeT<T>::operator+=(const RangeT<T>& range)
{
    *this = *this + range;
    return *this;
}

template <typename T>
inline RangeT<T>& RangeT<T>::operator*=(const RangeT<T>& range)
{
    *this = *this * range;
    return *this;
}

} // namespace utils
} // namespace simplex

#endif // UTILS_RANGE_H

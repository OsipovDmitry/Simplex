#ifndef UTILS_ORIENTEDBOUNDINGBOX_H
#define UTILS_ORIENTEDBOUNDINGBOX_H

#include <bitset>
#include <vector>

#include <utils/boundingbox.h>
#include <utils/combinatorics.h>
#include <utils/forwarddecl.h>
#include <utils/glm/detail/type_vec1.hpp>
#include <utils/glm/detail/type_vec2.hpp>
#include <utils/glm/detail/type_vec3.hpp>
#include <utils/transform.h>

namespace simplex
{
namespace utils
{

template <glm::length_t L, typename T>
struct OrientedBoundingBoxT
{
    static_assert((L >= 1) && (L < 4), "The dimaension of OientedBoundingBoxT must be [1..3]");
    static_assert(std::numeric_limits<T>::is_iec559, "The base type of OrientedBoundingBox must be floating point");

public:
    static constexpr size_t length() { return L; }
    static constexpr size_t numPoints() { return numberOfKDimFacesOfNDimBox(0ULL, length()); }
    static constexpr size_t numEdges() { return numberOfKDimFacesOfNDimBox(1ULL, length()); }
    static constexpr size_t numFaces() { return numberOfKDimFacesOfNDimBox(2ULL, length()); }
    using value_type = T;
    using PointType = glm::vec<length(), T>;
    using QuatType = glm::qua<T>;
    using EdgeType = LineSegmentT<length(), T>;

    OrientedBoundingBoxT()
        : OrientedBoundingBoxT<L, T>(empty())
    {
    }
    OrientedBoundingBoxT(const QuatType& rotation, const PointType& translation, const PointType& halfSizes)
        : m_rotation(rotation)
        , m_translation(translation)
        , m_halfSizes(halfSizes)
    {
    }
    OrientedBoundingBoxT(const BoundingBoxT<L, T>& boundingBox)
        : m_rotation(QuatType(static_cast<T>(1), PointType(static_cast<T>(0))))
        , m_translation(boundingBox.center())
        , m_halfSizes(boundingBox.halfSizes())
    {
    }

    const QuatType& rotation() const { return m_rotation; }
    const PointType& translation() const { return m_translation; }
    const PointType& halfSizes() const { return m_halfSizes; }

    bool isEmpty() const;

    PointType axis(size_t index) const
    {
        PointType localAxis(static_cast<value_type>(0));
        localAxis[index] = static_cast<value_type>(1);

        return PointType(m_rotation * vec3_cast(localAxis));
    }

    operator BoundingBoxT<length(), value_type>()
    {
        if (isEmpty()) return BoundingBoxT<length(), value_type>::empty();

        std::array<PointType, length()> axes;
        for (size_t i = 0u; i < length(); ++i)
            axes[i] = axis(i);

        PointType extents(static_cast<value_type>(0));
        for (size_t i = 0u; i < length(); ++i)
            for (size_t j = 0u; j < length(); ++j)
            {
                extents[i] += glm::abs(axes[j][i]) * m_halfSizes[j];
            }

        return BoundingBoxT<length(), value_type>::fromMinMax(m_translation - extents, m_translation + extents);
    }

    static OrientedBoundingBoxT<L, T> empty()
    {
        return OrientedBoundingBoxT<L, T>(
            QuatType(static_cast<value_type>(1), PointType(static_cast<value_type>(0))), PointType(static_cast<value_type>(0)),
            PointType(-std::numeric_limits<value_type>::max()));
    }

private:
    QuatType m_rotation;
    PointType m_translation;
    PointType m_halfSizes;
};

template <glm::length_t L, typename T>
inline OrientedBoundingBoxT<L, T> operator*(const TransformT<L, T>& t, const OrientedBoundingBoxT<L, T>& b)
{
    return OrientedBoundingBox(
        {t.transformQuat(b.rotation()), t.transformPoint(b.translation()), t.transformSize(b.halfSizes())});
}

template <glm::length_t L, typename T>
inline bool OrientedBoundingBoxT<L, T>::isEmpty() const
{
    for (glm::length_t k = 0; k < L; ++k)
        if (halfSizes()[k] < static_cast<T>(0)) return true;
    return false;
}

} // namespace utils
} // namespace simplex

#endif // UTILS_BOUNDINGBOX_H

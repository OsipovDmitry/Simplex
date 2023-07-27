#ifndef UTILS_TRANSFORM_H
#define UTILS_TRANSFORM_H

#include <utils/glm/detail/type_vec1.hpp>
#include <utils/glm/detail/type_vec2.hpp>
#include <utils/glm/detail/type_vec3.hpp>
#include <utils/glm/gtx/quaternion.hpp>
#include <utils/glm/detail/type_mat2x2.hpp>
#include <utils/glm/detail/type_mat3x3.hpp>
#include <utils/glm/detail/type_mat4x4.hpp>

#include <utils/forwarddecl.h>

namespace simplex
{
namespace utils
{

template <typename T> inline glm::vec<3, T> vec3cast(const glm::vec<1, T> &v) { return glm::vec<3, T>(v, 0.f, 0.f); }
template <typename T> inline glm::vec<3, T> vec3cast(const glm::vec<2, T> &v) { return glm::vec<3, T>(v, 0.f); }
template <typename T> inline glm::vec<3, T> vec3cast(const glm::vec<3, T> &v) { return v; }

template<glm::length_t L, typename T>
struct TransformT
{
    static_assert((L >= 1) && (L < 4), "The dimaension of Transform must be [1..3]");
    static_assert(std::numeric_limits<T>::is_iec559, "The base type of Transform must be floating point");

public:
    static constexpr glm::length_t length() { return L; }
    using value_type = T;

    using PointType = glm::vec<L, T>;
    using QuatType = glm::qua<T>;
    using MatType = glm::mat<L+1, L+1, T>;

    QuatType rotation;
    PointType translation;
    T scale;

    explicit TransformT(T s = 1.0, const QuatType &r = QuatType(1.0, 0.0, 0.0, 0.0), const PointType &t = PointType(0.0))
        : rotation(r), translation(t), scale(s) {}

    PointType operator *(const PointType &v) const
    {
        return translation + PointType(rotation * vec3cast(scale * v));
    }

    TransformT<L, T> &operator *=(const TransformT<L, T> &t2) {
        translation += scale * PointType(rotation * vec3cast(t2.translation));
        rotation *= t2.rotation;
        scale *= t2.scale;
        return *this;
    }

    TransformT<L, T> operator *(const TransformT<L, T> &t2) const {
        return TransformT(*this) *= t2;
    }

    TransformT<L, T> &invert()
    {
        scale = 1.f / scale;
        rotation = glm::conjugate(rotation);
        translation = -PointType(rotation  * vec3cast(scale * translation));
        return *this;
    }

    TransformT<L, T> inverted() const
    {
        return TransformT<L, T>(*this).invert();
    }

    operator MatType() const {
        return glm::translate(glm::mat<4, 4, T>(1.0), vec3cast(translation)) *
               glm::mat<4, 4, T>(rotation) *
               glm::scale(glm::mat<4, 4, T>(1.0), glm::vec<3, T>(scale));
    }

    static TransformT<L, T> fromScale(float value) { return TransformT<L, T>(value, QuatType(1.f, 0.f, 0.f, 0.f), PointType(0.0)); }
    static TransformT<L, T> fromRotation(const QuatType &value) { return TransformT<L, T>(1.0, value, PointType(0.0)); }
    static TransformT<L, T> fromTranslation(const PointType &value) { return TransformT<L, T>(1.0, QuatType(1.0, 0.0, 0.0, 0.0), value); }
};

template<glm::length_t L, typename T>
inline typename TransformT<L, T>::MatType operator *(const typename TransformT<L, T>::MatType &left, const TransformT<L, T> &right)
{
    return left * typename TransformT<L, T>::MatType(right);
}

template<glm::length_t L, typename T>
inline typename TransformT<L, T>::MatType operator *(const TransformT<L, T> &left, const typename TransformT<L, T>::MatType &right)
{
    return typename TransformT<L, T>::MatType(left) * right;
}

} // namespace
} // namespace

#endif // UTILS_TRANSFORM_H

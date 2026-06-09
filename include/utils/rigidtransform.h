#ifndef UTILS_RIGIDTRANSFORM_H
#define UTILS_RIGIDTRANSFORM_H

#include <utils/forwarddecl.h>
#include <utils/glm/detail/type_mat2x2.hpp>
#include <utils/glm/detail/type_mat3x3.hpp>
#include <utils/glm/detail/type_mat4x4.hpp>
#include <utils/glm/detail/type_vec1.hpp>
#include <utils/glm/detail/type_vec2.hpp>
#include <utils/glm/detail/type_vec3.hpp>
#include <utils/glm/gtx/quaternion.hpp>

namespace simplex
{
namespace utils
{

template <typename T>
inline glm::vec<3, T> vec3_cast(const glm::vec<1, T>& v)
{
    return glm::vec<3, T>(v, 0.f, 0.f);
}
template <typename T>
inline glm::vec<3, T> vec3_cast(const glm::vec<2, T>& v)
{
    return glm::vec<3, T>(v, 0.f);
}
template <typename T>
inline glm::vec<3, T> vec3_cast(const glm::vec<3, T>& v)
{
    return v;
}

template <glm::length_t L, typename T>
inline glm::mat<L + 1, L + 1, T> rotationMatrix(const glm::qua<T>& q)
{
    const auto mat3x3 = glm::mat3_cast(q);
    glm::mat<L + 1, L + 1, T> result(static_cast<T>(1));
    for (glm::length_t i = 0; i < L; ++i)
        for (glm::length_t j = 0; j < L; ++j)
            result[i][j] = mat3x3[i][j];
    return result;
}

template <glm::length_t L, typename T>
inline glm::mat<L + 1, L + 1, T> translationMatrix(const glm::vec<L, T>& v)
{
    glm::mat<L + 1, L + 1, T> result(static_cast<T>(1));
    result[L] = glm::vec<L + 1, T>(v, static_cast<T>(1));
    return result;
}

template <glm::length_t L, typename T>
struct RigidTransformT
{
    static_assert((L >= 1) && (L < 4), "The dimaension of Transform must be [1..3]");
    static_assert(std::numeric_limits<T>::is_iec559, "The base type of Transform must be floating point");

public:
    static constexpr glm::length_t length() { return L; }
    using value_type = T;
    using PointType = glm::vec<L, T>;
    using QuatType = glm::qua<T>;
    using MatrixType = glm::mat<L + 1, L + 1, T>;

    QuatType rotation;
    PointType translation;

    explicit RigidTransformT(
        const QuatType& = QuatType(static_cast<T>(1), PointType(static_cast<T>(0))),
        const PointType& = PointType(static_cast<T>(0)));

    PointType transformVector(const PointType&) const;
    QuatType transformQuat(const QuatType&) const;
    PointType transformPoint(const PointType&) const;

    PointType operator*(const PointType&) const;
    RigidTransformT<L, T>& operator*=(const RigidTransformT<L, T>&);
    RigidTransformT<L, T> operator*(const RigidTransformT<L, T>&) const;

    RigidTransformT<L, T>& invert();
    RigidTransformT<L, T> inverted() const;

    operator MatrixType() const;

    static RigidTransformT<L, T> makeIdentity();
    static RigidTransformT<L, T> makeRotation(const QuatType&);
    static RigidTransformT<L, T> makeTranslation(const PointType&);
    static RigidTransformT<L, T> makeLookAt(const PointType&, const PointType&, const PointType&);
};

template <glm::length_t L, typename T>
inline RigidTransformT<L, T>::RigidTransformT(const QuatType& r, const PointType& t)
    : rotation(r)
    , translation(t)
{
}

template <glm::length_t L, typename T>
inline typename RigidTransformT<L, T>::PointType RigidTransformT<L, T>::transformVector(const PointType& v) const
{
    return rotation * vec3_cast(v);
}

template <glm::length_t L, typename T>
inline typename RigidTransformT<L, T>::QuatType RigidTransformT<L, T>::transformQuat(const QuatType& q) const
{
    return rotation * q;
}

template <glm::length_t L, typename T>
inline typename RigidTransformT<L, T>::PointType RigidTransformT<L, T>::transformPoint(const PointType& p) const
{
    return translation + PointType(rotation * vec3_cast(p));
}

template <glm::length_t L, typename T>
inline typename RigidTransformT<L, T>::PointType RigidTransformT<L, T>::operator*(const PointType& p) const
{
    return transformPoint(p);
}

template <glm::length_t L, typename T>
inline RigidTransformT<L, T>& RigidTransformT<L, T>::operator*=(const RigidTransformT<L, T>& t2)
{
    translation += PointType(rotation * vec3_cast(t2.translation));
    rotation *= t2.rotation;
    return *this;
}

template <glm::length_t L, typename T>
inline RigidTransformT<L, T> RigidTransformT<L, T>::operator*(const RigidTransformT<L, T>& t2) const
{
    return RigidTransformT(*this) *= t2;
}

template <glm::length_t L, typename T>
inline RigidTransformT<L, T>& RigidTransformT<L, T>::invert()
{
    rotation = glm::conjugate(rotation);
    translation = -PointType(rotation * vec3_cast(translation));
    return *this;
}

template <glm::length_t L, typename T>
inline RigidTransformT<L, T> RigidTransformT<L, T>::inverted() const
{
    return RigidTransformT<L, T>(*this).invert();
}

template <glm::length_t L, typename T>
inline RigidTransformT<L, T>::operator MatrixType() const
{
    return translationMatrix(translation) * rotationMatrix<L>(rotation);
}

template <glm::length_t L, typename T>
inline RigidTransformT<L, T> RigidTransformT<L, T>::makeIdentity()
{
    return RigidTransformT<L, T>();
}

template <glm::length_t L, typename T>
inline RigidTransformT<L, T> RigidTransformT<L, T>::makeRotation(const QuatType& value)
{
    return RigidTransformT<L, T>(value, PointType(static_cast<T>(0)));
}

template <glm::length_t L, typename T>
inline RigidTransformT<L, T> RigidTransformT<L, T>::makeTranslation(const PointType& value)
{
    return RigidTransformT<L, T>(QuatType(static_cast<T>(1), PointType(static_cast<T>(0))), value);
}

template <glm::length_t L, typename T>
inline RigidTransformT<L, T> RigidTransformT<L, T>::makeLookAt(const PointType& eye, const PointType& center, const PointType& up)
{
    const auto q = glm::conjugate(glm::quatLookAt(glm::normalize(vec3_cast(center - eye)), glm::normalize(vec3_cast(up))));
    return RigidTransformT<L, T>(q, -(q * vec3_cast(eye)));
}

template <glm::length_t L, typename T>
inline typename RigidTransformT<L, T>::MatrixType operator*(
    const typename RigidTransformT<L, T>::MatrixType& left,
    const RigidTransformT<L, T>& right)
{
    return left * typename RigidTransformT<L, T>::MatrixType(right);
}

template <glm::length_t L, typename T>
inline typename RigidTransformT<L, T>::MatrixType operator*(
    const RigidTransformT<L, T>& left,
    const typename RigidTransformT<L, T>::MatrixType& right)
{
    return typename RigidTransformT<L, T>::MatrixType(left) * right;
}

} // namespace utils
} // namespace simplex

#endif // UTILS_RIGIDTRANSFORM_H

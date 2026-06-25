#ifndef UTILS_TRANSFORM_H
#define UTILS_TRANSFORM_H

#include <utils/rigidtransform.h>

namespace simplex
{
namespace utils
{

template <glm::length_t L, typename T>
inline glm::mat<L + 1, L + 1, T> scaleMatrix(T s)
{
    glm::mat<L + 1, L + 1, T> result(static_cast<T>(1));
    for (glm::length_t i = 0; i < L; ++i)
        result[i][i] = s;
    return result;
}

template <glm::length_t L, typename T>
struct TransformT
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
    T scale;

    explicit TransformT(
        T = static_cast<T>(1),
        const QuatType& = QuatType(static_cast<T>(1), PointType(static_cast<T>(0))),
        const PointType& = PointType(static_cast<T>(0)));
    explicit TransformT(const RigidTransformT<L, T>&);

    PointType transformVector(const PointType&) const;
    QuatType transformQuat(const QuatType&) const;
    PointType transformPoint(const PointType&) const;
    PointType transformSize(const PointType&) const;
    value_type transformSize(value_type) const;

    PointType operator*(const PointType&) const;
    TransformT<L, T>& operator*=(const TransformT<L, T>&);
    TransformT<L, T> operator*(const TransformT<L, T>&) const;

    TransformT<L, T>& invert();
    TransformT<L, T> inverted() const;

    operator MatrixType() const;

    static TransformT<L, T> makeIdentity();
    static TransformT<L, T> makeScale(T);
    static TransformT<L, T> makeRotation(const QuatType&);
    static TransformT<L, T> makeTranslation(const PointType&);
    static TransformT<L, T> makeLookAt(const PointType&, const PointType&, const PointType&);
};

template <glm::length_t L, typename T>
inline TransformT<L, T>::TransformT(T s, const QuatType& r, const PointType& t)
    : rotation(r)
    , translation(t)
    , scale(s)
{
}

template <glm::length_t L, typename T>
inline TransformT<L, T>::TransformT(const RigidTransformT<L, T>& rt)
    : rotation(rt.rotation)
    , translation(rt.translation)
    , scale(static_cast<T>(1))
{
}

template <glm::length_t L, typename T>
inline typename TransformT<L, T>::PointType TransformT<L, T>::transformVector(const PointType& v) const
{
    return rotation * vec3_cast(v);
}

template <glm::length_t L, typename T>
inline typename TransformT<L, T>::QuatType TransformT<L, T>::transformQuat(const QuatType& q) const
{
    return rotation * q;
}

template <glm::length_t L, typename T>
inline typename TransformT<L, T>::PointType TransformT<L, T>::transformPoint(const PointType& p) const
{
    return translation + PointType(rotation * vec3_cast(scale * p));
}

template <glm::length_t L, typename T>
inline typename TransformT<L, T>::PointType TransformT<L, T>::transformSize(const PointType& s) const
{
    return scale * s;
}

template <glm::length_t L, typename T>
inline typename TransformT<L, T>::value_type TransformT<L, T>::transformSize(value_type s) const
{
    return scale * s;
}

template <glm::length_t L, typename T>
inline typename TransformT<L, T>::PointType TransformT<L, T>::operator*(const PointType& p) const
{
    return transformPoint(p);
}

template <glm::length_t L, typename T>
inline TransformT<L, T>& TransformT<L, T>::operator*=(const TransformT<L, T>& t2)
{
    translation += scale * PointType(rotation * vec3_cast(t2.translation));
    rotation *= t2.rotation;
    scale *= t2.scale;
    return *this;
}

template <glm::length_t L, typename T>
inline TransformT<L, T>& TransformT<L, T>::invert()
{
    scale = static_cast<T>(1) / scale;
    rotation = glm::conjugate(rotation);
    translation = -PointType(rotation * vec3_cast(scale * translation));
    return *this;
}

template <glm::length_t L, typename T>
inline TransformT<L, T> TransformT<L, T>::inverted() const
{
    return TransformT<L, T>(*this).invert();
}

template <glm::length_t L, typename T>
inline TransformT<L, T>::operator MatrixType() const
{
    return translationMatrix(translation) * rotationMatrix<L>(rotation) * scaleMatrix<L>(scale);
}

template <glm::length_t L, typename T>
inline TransformT<L, T> TransformT<L, T>::makeIdentity()
{
    return TransformT<L, T>();
}

template <glm::length_t L, typename T>
inline TransformT<L, T> TransformT<L, T>::makeScale(T value)
{
    return TransformT<L, T>(value, QuatType(static_cast<T>(1), PointType(static_cast<T>(0))), PointType(static_cast<T>(0)));
}

template <glm::length_t L, typename T>
inline TransformT<L, T> TransformT<L, T>::makeRotation(const QuatType& value)
{
    return TransformT<L, T>(static_cast<T>(1), value, PointType(static_cast<T>(0)));
}

template <glm::length_t L, typename T>
inline TransformT<L, T> TransformT<L, T>::makeTranslation(const PointType& value)
{
    return TransformT<L, T>(static_cast<T>(1), QuatType(static_cast<T>(1), PointType(static_cast<T>(0))), value);
}

template <glm::length_t L, typename T>
inline TransformT<L, T> TransformT<L, T>::makeLookAt(const PointType& eye, const PointType& center, const PointType& up)
{
    const auto q = glm::conjugate(glm::quatLookAt(glm::normalize(vec3_cast(center - eye)), glm::normalize(vec3_cast(up))));
    return TransformT<L, T>(static_cast<T>(1), q, -(q * vec3_cast(eye)));
}

template <glm::length_t L, typename T>
inline TransformT<L, T> TransformT<L, T>::operator*(const TransformT<L, T>& t2) const
{
    return TransformT(*this) *= t2;
}

template <glm::length_t L, typename T>
inline typename TransformT<L, T>::MatrixType operator*(
    const typename TransformT<L, T>::MatrixType& left,
    const TransformT<L, T>& right)
{
    return left * typename TransformT<L, T>::MatrixType(right);
}

template <glm::length_t L, typename T>
inline typename TransformT<L, T>::MatrixType operator*(
    const TransformT<L, T>& left,
    const typename TransformT<L, T>::MatrixType& right)
{
    return typename TransformT<L, T>::MatrixType(left) * right;
}

} // namespace utils
} // namespace simplex

#endif // UTILS_TRANSFORM_H

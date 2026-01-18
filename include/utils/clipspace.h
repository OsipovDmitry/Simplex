#ifndef UTILS_CLIPSPACE_H
#define UTILS_CLIPSPACE_H

#include <memory>

#include <utils/glm/gtc/matrix_transform.hpp>
#include <utils/forwarddecl.h>
#include <utils/enumclass.h>
#include <utils/range.h>

namespace simplex
{
namespace utils
{

ENUMCLASS(ClipSpaceType, uint16_t, Ortho, Perspective, Spherical)

template <typename T>
struct ClipSpaceT
{
    static_assert(std::numeric_limits<T>::is_iec559, "The base type of ClipSpace must be floating point");
public:
    static constexpr size_t length() { return 3u; }
    using value_type = T;
    using MatrixType = glm::mat<length() + 1u, length() + 1u, T>;

    explicit ClipSpaceT();

    ClipSpaceType type() const;
    const glm::vec<4u, T>& params() const;

    MatrixType projectionMatrix(const utils::RangeT<T>&) const;

    static ClipSpaceT<T> makeIdentity();
    static ClipSpaceT<T> makeOrtho(T left, T right, T bottom, T top);
    static ClipSpaceT<T> makeOrtho(T aspectRatio, T height);
    static ClipSpaceT<T> makePerspective(T left, T right, T bottom, T top); // left, right, bottom, top have to be calculated for zNear == 1.0
    static ClipSpaceT<T> makePerspective(T aspectRatio, T fovY);
    static ClipSpaceT<T> makeSpherical();

protected:
    ClipSpaceT(ClipSpaceType type, T left, T right, T bottom, T top);

    ClipSpaceType m_type;
    glm::vec<4u, T> m_params;

};

template <typename T>
inline ClipSpaceT<T>::ClipSpaceT()
    : ClipSpaceT<T>(makeIdentity())
{
}

template<typename T>
inline ClipSpaceType ClipSpaceT<T>::type() const
{
    return m_type;
}

template<typename T>
inline const glm::vec<4u, T>& ClipSpaceT<T>::params() const
{
    return m_params;
}

template<typename T>
inline typename ClipSpaceT<T>::MatrixType ClipSpaceT<T>::projectionMatrix(const utils::RangeT<T> &zRange) const
{
    MatrixType result(1.f);

    if (zRange.isEmpty())
        return result;

    switch (m_type)
    {
    case ClipSpaceType::Ortho: {
        result =  glm::ortho(
            m_params[0u],
            m_params[1u],
            m_params[2u],
            m_params[3u],
            zRange.nearValue(),
            zRange.farValue());
        break;
    }
    case ClipSpaceType::Perspective: {
        const auto &zNear = zRange.nearValue();
        result = glm::frustum(
            m_params[0u] * zNear,
            m_params[1u] * zNear,
            m_params[2u] * zNear,
            m_params[3u] * zNear,
            zRange.nearValue(),
            zRange.farValue());
        break;
    }
    case ClipSpaceType::Spherical: {
        const auto farInverse = static_cast<T>(1) / zRange.farValue();
        result = glm::scale(glm::mat<4, 4, T>(static_cast<T>(1)), glm::vec<3, T>(farInverse, farInverse, -farInverse));
        break;
    }
    default: {
        break;
    }
    }

    return result;
}

template <typename T>
inline ClipSpaceT<T> ClipSpaceT<T>::makeIdentity()
{
    return ClipSpaceT<T>(ClipSpaceType::Ortho, static_cast<T>(-1), static_cast<T>(1), static_cast<T>(-1), static_cast<T>(1));
}

template <typename T>
inline ClipSpaceT<T> ClipSpaceT<T>::makeOrtho(T left, T right, T bottom, T top)
{
    return ClipSpaceT<T>(ClipSpaceType::Ortho, left, right, bottom, top);
}

template <typename T>
inline ClipSpaceT<T> ClipSpaceT<T>::makeOrtho(T aspctRatio, T height)
{
    const auto fH = height * static_cast<T>(.5);
    const auto fW = fH * aspctRatio;
    return makeOrtho(-fW, fW, -fH, fH);
}

template <typename T>
inline ClipSpaceT<T> ClipSpaceT<T>::makePerspective(T left, T right, T bottom, T top)
{
    return ClipSpaceT<T>(ClipSpaceType::Perspective, left, right, bottom, top);
}

template <typename T>
inline ClipSpaceT<T> ClipSpaceT<T>::makePerspective(T aspctRatio, T fovY)
{
    const auto fH = glm::tan(fovY / static_cast<T>(2));
    const auto fW = fH * aspctRatio;
    return makePerspective(-fW, fW, -fH, fH);
}

template <typename T>
inline ClipSpaceT<T> ClipSpaceT<T>::makeSpherical()
{
    return ClipSpaceT<T>(ClipSpaceType::Spherical, static_cast<T>(-1), static_cast<T>(1), static_cast<T>(-1), static_cast<T>(1));
}

template<typename T>
inline ClipSpaceT<T>::ClipSpaceT(ClipSpaceType type, T left, T right, T bottom, T top)
    : m_type(type)
    , m_params(left, right, bottom, top)
{
}

} // namespace
} // namespace

#endif // UTILS_CLIPSPACE_H

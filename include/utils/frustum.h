#ifndef UTILS_FRUSTUM_H
#define UTILS_FRUSTUM_H

#include <vector>

#include <utils/glm/detail/type_mat4x4.hpp>
#include <utils/enumclass.h>
#include <utils/forwarddecl.h>
#include <utils/transform.h>
#include <utils/plane.h>
#include <utils/boundingbox.h>

namespace simplex
{
namespace utils
{

template<typename T>
struct FrustumCornersInfoT
{
    std::array<glm::vec<3u, T>, 8u> corners;
    glm::vec<3u, T> center;
};

template<typename T>
struct FrustumT
{
    static_assert(std::numeric_limits<T>::is_iec559, "The base type of Frustum must be floating point");

public:
    static constexpr glm::length_t length() { return 3; }
    using value_type = T;
    using PlaneType = PlaneT<3u, T>;

    ENUMCLASS(Planes, uint16_t, Left, Right, Bottom, Top, Near, Far)

    FrustumT()
        : planes(numElementsPlanes())
    {}
    FrustumT(const glm::mat<4u, 4u, T> &vp)
    {
        planes.resize(numElementsPlanes());
        planes[castFromPlanes(Planes::Left)] = PlaneType(glm::vec<4, T>(vp[0][3] + vp[0][0], vp[1][3] + vp[1][0], vp[2][3] + vp[2][0], vp[3][3] + vp[3][0]));
        planes[castFromPlanes(Planes::Right)] = Plane(glm::vec<4, T>(vp[0][3] - vp[0][0], vp[1][3] - vp[1][0], vp[2][3] - vp[2][0], vp[3][3] - vp[3][0]));
        planes[castFromPlanes(Planes::Bottom)] = Plane(glm::vec<4, T>(vp[0][3] + vp[0][1], vp[1][3] + vp[1][1], vp[2][3] + vp[2][1], vp[3][3] + vp[3][1]));
        planes[castFromPlanes(Planes::Top)] = Plane(glm::vec<4, T>(vp[0][3] - vp[0][1], vp[1][3] - vp[1][1], vp[2][3] - vp[2][1], vp[3][3] - vp[3][1]));
        planes[castFromPlanes(Planes::Near)] = Plane(glm::vec<4, T>(vp[0][3] + vp[0][2], vp[1][3] + vp[1][2], vp[2][3] + vp[2][2], vp[3][3] + vp[3][2]));
        planes[castFromPlanes(Planes::Far)] = Plane(glm::vec<4, T>(vp[0][3] - vp[0][2], vp[1][3] - vp[1][2], vp[2][3] - vp[2][2], vp[3][3] - vp[3][2]));
    }

    bool contain(const BoundingBoxT<length(), T> &bb) const {
        if (bb.isEmpty())
            return false;
        for (const auto &plane : planes)
            if (bb.distanceToPlane(plane) < static_cast<T>(0))
                return false;
        return true;
    }

    static FrustumCornersInfoT<T> calculateCornersInfo(const glm::mat<4, 4, T> &vpInverse)
    {
        static const std::array<glm::vec<4u, T>, 8u> s_frustumCorners {
            glm::vec<4u, T>(static_cast<T>(-1), static_cast<T>(1), static_cast<T>(-1), static_cast<T>(1)),
            glm::vec<4u, T>(static_cast<T>(1), static_cast<T>(1), static_cast<T>(-1), static_cast<T>(1)),
            glm::vec<4u, T>(static_cast<T>(1), static_cast<T>(-1), static_cast<T>(-1), static_cast<T>(1)),
            glm::vec<4u, T>(static_cast<T>(-1), static_cast<T>(-1), static_cast<T>(-1), static_cast<T>(1)),
            glm::vec<4u, T>(static_cast<T>(-1), static_cast<T>(1), static_cast<T>(1), static_cast<T>(1)),
            glm::vec<4u, T>(static_cast<T>(1), static_cast<T>(1), static_cast<T>(1), static_cast<T>(1)),
            glm::vec<4u, T>(static_cast<T>(1), static_cast<T>(-1), static_cast<T>(1), static_cast<T>(1)),
            glm::vec<4u, T>(static_cast<T>(-1), static_cast<T>(-1), static_cast<T>(1), static_cast<T>(1))
        };

        std::array<glm::vec<3u, T>, 8u> corners;
        glm::vec<3u, T> center(static_cast<T>(0));

        for (uint32_t i = 0u; i < s_frustumCorners.size(); ++i)
        {
            auto corner = vpInverse * s_frustumCorners[i];
            corners[i] = glm::vec<3u, T>(corner / corner[3u]);
            center += corners[i];
        }

        center /= static_cast<T>(corners.size());

        return {corners, center};
    }

    std::vector<PlaneType> planes;
};

template<typename T>
inline FrustumT<T> operator *(const TransformT<FrustumT<T>::length(), T> &t, const FrustumT<T> &f)
{
    FrustumT transformedFrustum = f;

    for (auto &p : transformedFrustum.planes)
        p = t * p;

    return transformedFrustum;
}

template<typename T>
struct OpenFrustumT : public FrustumT<T>
{
    OpenFrustumT(const glm::mat<4u, 4u, T> &vp)
        : FrustumT<T>(vp)
    {
        FrustumT<T>::planes.resize(FrustumT<T>::numElementsPlanes() - 1u); // Far plane is not needed for open FrustumT. Remove it.
    }
};

} // namespace
} // namespace

#endif // UTILS_FRUSTUM_H

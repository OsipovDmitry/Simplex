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
struct FrustumT
{
    static_assert(std::numeric_limits<T>::is_iec559, "The base type of Frustum must be floating point");

public:
    static constexpr glm::length_t length() { return 3; }
    using value_type = T;
    using PlaneType = PlaneT<3, T>;

    ENUMCLASS(Planes, size_t, Left, Right, Bottom, Top, Near, Far)

    FrustumT()
        : planes(numElementsPlanes())
    {}
    FrustumT(const glm::mat<4, 4, T> &vp)
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
            if (bb.distanceToPlane(plane) < .0f)
                return false;
        return true;
    }

    std::vector<PlaneType> planes;
};

template<typename T>
inline FrustumT<T> operator *(const TransformT<FrustumT<T>::length(), T> &t, const FrustumT<T> &f)
{
    FrustumT transformedFrustumT = f;

    for (auto &p : transformedFrustumT.planes)
        p = t * p;

    //    for (auto &v : transformedFrustumT.vertices)
    //        v = t * v;

    return transformedFrustumT;
}

template<typename T>
struct OpenFrustumT : public FrustumT<T>
{
    OpenFrustumT(const glm::mat<4, 4, T> &vp)
        : FrustumT<T>(vp)
    {
        FrustumT<T>::planes.resize(FrustumT<T>::numElementsPlanes() - 1u); // Far plane is not needed for open FrustumT. Remove it.
    }
};

} // namespace
} // namespace

#endif // UTILS_FRUSTUM_H

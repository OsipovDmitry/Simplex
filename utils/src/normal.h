#ifndef UTILS_TANGENTSPACE_H
#define UTILS_TANGENTSPACE_H

#include <vector>

#include <utils/glm/matrix.hpp>
#include <utils/glm/gtc/type_ptr.hpp>
#include <utils/glm/gtx/normal.hpp>
#include <utils/primitiveset.h>


namespace simplex
{
namespace utils
{

template<typename V, typename I>
inline void calculateNormalForTriangle(uint32_t verticesNumComponents,
                                       const V *vertices,
                                       const I &i0,
                                       const I &i1,
                                       const I &i2,
                                       uint32_t normalsNumComponents,
                                       V *normals)
{
    const auto &v0 = *reinterpret_cast<const glm::vec<3u, V>*>(vertices + i0 * verticesNumComponents);
    const auto &v1 = *reinterpret_cast<const glm::vec<3u, V>*>(vertices + i1 * verticesNumComponents);
    const auto &v2 = *reinterpret_cast<const glm::vec<3u, V>*>(vertices + i2 * verticesNumComponents);

    const auto N = glm::triangleNormal(v0, v1, v2);

    *reinterpret_cast<glm::vec<3u, V>*>(normals + i0 * normalsNumComponents) += N;
    *reinterpret_cast<glm::vec<3u, V>*>(normals + i1 * normalsNumComponents) += N;
    *reinterpret_cast<glm::vec<3u, V>*>(normals + i2 * normalsNumComponents) += N;
}

template<typename V>
inline void calculateNormal(const std::shared_ptr<DrawArrays> &drawArrays,
                            uint32_t verticesNumComponents,
                            const V *vertices,
                            uint32_t normalsNumComponents,
                            V *normals)
{
    assert(drawArrays->primitiveType() == PrimitiveType::Triangles);

    assert(verticesNumComponents >= 3);
    assert(normalsNumComponents >= 3);

    const auto drawArraysFirst = drawArrays->first();
    const auto drawArraysCount = drawArrays->count();

    for (uint32_t i = 0u; i < drawArraysCount; i += 3)
    {
        const uint32_t i0 = i + drawArraysFirst + 0u;
        const uint32_t i1 = i + drawArraysFirst + 1u;
        const uint32_t i2 = i + drawArraysFirst + 2u;
        calculateNormalForTriangle(verticesNumComponents,
                                   vertices,
                                   i0,
                                   i1,
                                   i2,
                                   normalsNumComponents,
                                   normals);
    }
}

template<typename V, typename I>
inline void calculateNormal(const std::shared_ptr<DrawElements> &drawElements,
                                  uint32_t verticesNumComponents,
                                  const V *vertices,
                                  const I *indices,
                                  uint32_t normalsNumComponents,
                                  V *normals)
{
    assert(drawElements->primitiveType() == PrimitiveType::Triangles);

    assert(verticesNumComponents >= 3);
    assert(normalsNumComponents >= 3);

    const auto drawElementsCount = drawElements->count();
    const auto drawElementsBaseVertex = drawElements->baseVertex();
    const auto *pIndices = reinterpret_cast<const I*>(reinterpret_cast<const uint8_t*>(indices) + drawElements->offset());

    for (uint32_t i = 0u; i < drawElementsCount; i += 3)
    {
        const I i0 = static_cast<I>(pIndices[i + 0u] + drawElementsBaseVertex);
        const I i1 = static_cast<I>(pIndices[i + 1u] + drawElementsBaseVertex);
        const I i2 = static_cast<I>(pIndices[i + 2u] + drawElementsBaseVertex);
        calculateNormalForTriangle(verticesNumComponents,
                                   vertices,
                                   i0,
                                   i1,
                                   i2,
                                   normalsNumComponents,
                                   normals);
    }
}

template<typename V>
inline void normalize(uint32_t numVertices,
                      uint32_t normalsNumComponents,
                      const V *normals,
                      uint32_t resultNumComponents,
                      V *result)

{
    assert(normalsNumComponents >= 3);
    assert(resultNumComponents >= 3);

    for (uint32_t i = 0u; i < numVertices; ++i)
    {
        const auto &n = *reinterpret_cast<const glm::vec<3u, V>*>(normals + i * normalsNumComponents);

        *reinterpret_cast<glm::vec<3u, V>*>(result + i * resultNumComponents) = glm::normalize(n);
    }

}

template<typename V, typename I>
inline void calculateTangentSpaceForTriangle(uint32_t verticesNumComponents,
                                             const V *vertices,
                                             uint32_t texCoordsNumComponents,
                                             const V *texCoords,
                                             const I &i0,
                                             const I &i1,
                                             const I &i2,
                                             uint32_t tangentsNumComponents,
                                             V *tangents,
                                             uint32_t binormalsNumComponents,
                                             V *binormals)
{
    const auto &v0 = *reinterpret_cast<const glm::vec<3u, V>*>(vertices + i0 * verticesNumComponents);
    const auto &v1 = *reinterpret_cast<const glm::vec<3u, V>*>(vertices + i1 * verticesNumComponents);
    const auto &v2 = *reinterpret_cast<const glm::vec<3u, V>*>(vertices + i2 * verticesNumComponents);

    const auto &w0 = *reinterpret_cast<const glm::vec<2u, V>*>(texCoords + i0 * texCoordsNumComponents);
    const auto &w1 = *reinterpret_cast<const glm::vec<2u, V>*>(texCoords + i1 * texCoordsNumComponents);
    const auto &w2 = *reinterpret_cast<const glm::vec<2u, V>*>(texCoords + i2 * texCoordsNumComponents);

    const auto Q = glm::mat<2u, 3u, V>(v1 - v0, v2 - v0);
    const auto W = glm::mat<2u, 2u, V>(w1 - w0, w2 - w0);
    const auto TB = Q * glm::inverse(W);

    *reinterpret_cast<glm::vec<3u, V>*>(tangents + i0 * tangentsNumComponents) += TB[0u];
    *reinterpret_cast<glm::vec<3u, V>*>(tangents + i1 * tangentsNumComponents) += TB[0u];
    *reinterpret_cast<glm::vec<3u, V>*>(tangents + i2 * tangentsNumComponents) += TB[0u];

    *reinterpret_cast<glm::vec<3u, V>*>(binormals + i0 * binormalsNumComponents) += TB[1u];
    *reinterpret_cast<glm::vec<3u, V>*>(binormals + i1 * binormalsNumComponents) += TB[1u];
    *reinterpret_cast<glm::vec<3u, V>*>(binormals + i2 * binormalsNumComponents) += TB[1u];
}

template<typename V>
inline void calculateTangentSpace(const std::shared_ptr<DrawArrays> &drawArrays,
                                  uint32_t verticesNumComponents,
                                  const V *vertices,
                                  uint32_t texCoordsNumComponents,
                                  const V *texCoords,
                                  uint32_t tangentsNumComponents,
                                  V *tangents,
                                  uint32_t binormalsNumComponents,
                                  V *binormals)
{
    assert(drawArrays->primitiveType() == PrimitiveType::Triangles);

    assert(verticesNumComponents >= 3);
    assert(texCoordsNumComponents >= 2);
    assert(tangentsNumComponents >= 3);
    assert(binormalsNumComponents >= 3);

    const auto drawArraysFirst = drawArrays->first();
    const auto drawArraysCount = drawArrays->count();

    for (uint32_t i = 0u; i < drawArraysCount; i += 3)
    {
        const uint32_t i0 = i + drawArraysFirst + 0u;
        const uint32_t i1 = i + drawArraysFirst + 1u;
        const uint32_t i2 = i + drawArraysFirst + 2u;
        calculateTangentSpaceForTriangle(verticesNumComponents,
                                         vertices,
                                         texCoordsNumComponents,
                                         texCoords,
                                         i0,
                                         i1,
                                         i2,
                                         tangentsNumComponents,
                                         tangents,
                                         binormalsNumComponents,
                                         binormals);
    }
}

template<typename V, typename I>
inline void calculateTangentSpace(const std::shared_ptr<DrawElements> &drawElements,
                                  uint32_t verticesNumComponents,
                                  const V *vertices,
                                  uint32_t texCoordsNumComponents,
                                  const V *texCoords,
                                  const I *indices,
                                  uint32_t tangentsNumComponents,
                                  V *tangents,
                                  uint32_t binormalsNumComponents,
                                  V *binormals)
{
    assert(drawElements->primitiveType() == PrimitiveType::Triangles);

    assert(verticesNumComponents >= 3);
    assert(texCoordsNumComponents >= 2);
    assert(tangentsNumComponents >= 3);
    assert(binormalsNumComponents >= 3);

    const auto drawElementsCount = drawElements->count();
    const auto drawElementsBaseVertex = drawElements->baseVertex();
    const auto *pIndices = reinterpret_cast<const I*>(reinterpret_cast<const uint8_t*>(indices) + drawElements->offset());

    for (uint32_t i = 0u; i < drawElementsCount; i += 3)
    {
        const I i0 = static_cast<I>(pIndices[i + 0u] + drawElementsBaseVertex);
        const I i1 = static_cast<I>(pIndices[i + 1u] + drawElementsBaseVertex);
        const I i2 = static_cast<I>(pIndices[i + 2u] + drawElementsBaseVertex);
        calculateTangentSpaceForTriangle(verticesNumComponents,
                                         vertices,
                                         texCoordsNumComponents,
                                         texCoords,
                                         i0,
                                         i1,
                                         i2,
                                         tangentsNumComponents,
                                         tangents,
                                         binormalsNumComponents,
                                         binormals);
    }
}

template<typename V>
inline void orthogonalizeTangentSpace(uint32_t numVertices,
                                      uint32_t tangentsNumComponents,
                                      const V *tangents,
                                      uint32_t binormalsNumComponents,
                                      const V *binormals,
                                      uint32_t normalsNumComponents,
                                      const V *normals,
                                      uint32_t resultNumComponents,
                                      V *result)

{
    assert(tangentsNumComponents >= 3);
    assert(binormalsNumComponents >= 3);
    assert(normalsNumComponents >= 3);
    assert(resultNumComponents >= 4);

    for (uint32_t i = 0u; i < numVertices; ++i)
    {
        const auto &n = *reinterpret_cast<const glm::vec<3u, V>*>(normals + i * normalsNumComponents);
        const auto &t = *reinterpret_cast<const glm::vec<3u, V>*>(tangents + i * tangentsNumComponents);
        const auto &b = *reinterpret_cast<const glm::vec<3u, V>*>(binormals + i * binormalsNumComponents);

        glm::vec4 q =                 glm::vec<4u, V>(glm::normalize(t - n * glm::dot(t, n)),
                                                      (glm::dot(glm::cross(n, t), b) < static_cast<V>(0)) ?
                                                          static_cast<V>(-1) :
                                                          static_cast<V>(+1));

        *reinterpret_cast<glm::vec<4u, V>*>(result + i * resultNumComponents) = q;
    }

}

}
}

#endif // UTILS_TANGENTSPACE_H

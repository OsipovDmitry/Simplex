#ifndef UTILS_MESHIMPL_H
#define UTILS_MESHIMPL_H

#include <vector>

#include <utils/glm/matrix.hpp>
#include <utils/glm/gtc/type_ptr.hpp>
#include <utils/glm/gtx/normal.hpp>
#include <utils/logger.h>
#include <utils/boundingbox.h>
#include <utils/primitiveset.h>

namespace simplex
{
namespace utils
{

template<VertexComponentType DstType, VertexComponentType SrcType>
inline uint8_t* convertToVertexComponentType(
    const uint8_t* data,
    size_t numVertices,
    uint32_t numComponents)
{
    if constexpr (DstType != SrcType)
        LOG_CRITICAL << "Conversion between floating-point numbers and integers, between signed and unsigned integers and decrease the size of type is not allowed";
    return nullptr;
}

template<>
inline uint8_t* convertToVertexComponentType<VertexComponentType::Double, VertexComponentType::Single>(
    const uint8_t* data,
    size_t numVertices,
    uint32_t numComponents)
{
    using DstType = double;
    using SrcType = float;

    const auto totalNumComponents = numVertices * numComponents;
    auto dstData = new DstType[totalNumComponents];
    auto srcData = reinterpret_cast<const SrcType*>(data);
    for (size_t i = 0u; i < totalNumComponents; ++i)
        dstData[i] = srcData[i];
    return reinterpret_cast<uint8_t*>(dstData);
}

template<>
inline uint8_t* convertToVertexComponentType<VertexComponentType::Int16, VertexComponentType::Int8>(
    const uint8_t* data,
    size_t numVertices,
    uint32_t numComponents)
{
    using DstType = int16_t;
    using SrcType = int8_t;

    const auto totalNumComponents = numVertices * numComponents;
    auto dstData = new DstType[totalNumComponents];
    auto srcData = reinterpret_cast<const SrcType*>(data);
    for (size_t i = 0u; i < totalNumComponents; ++i)
        dstData[i] = srcData[i];
    return reinterpret_cast<uint8_t*>(dstData);
}

template<>
inline uint8_t* convertToVertexComponentType<VertexComponentType::Int32, VertexComponentType::Int8>(
    const uint8_t* data,
    size_t numVertices,
    uint32_t numComponents)
{
    using DstType = int32_t;
    using SrcType = int8_t;

    const auto totalNumComponents = numVertices * numComponents;
    auto dstData = new DstType[totalNumComponents];
    auto srcData = reinterpret_cast<const SrcType*>(data);
    for (size_t i = 0u; i < totalNumComponents; ++i)
        dstData[i] = srcData[i];
    return reinterpret_cast<uint8_t*>(dstData);
}

template<>
inline uint8_t* convertToVertexComponentType<VertexComponentType::Int32, VertexComponentType::Int16>(
    const uint8_t* data,
    size_t numVertices,
    uint32_t numComponents)
{
    using DstType = int32_t;
    using SrcType = int16_t;

    const auto totalNumComponents = numVertices * numComponents;
    auto dstData = new DstType[totalNumComponents];
    auto srcData = reinterpret_cast<const SrcType*>(data);
    for (size_t i = 0u; i < totalNumComponents; ++i)
        dstData[i] = srcData[i];
    return reinterpret_cast<uint8_t*>(dstData);
}

template<>
inline uint8_t* convertToVertexComponentType<VertexComponentType::Uint16, VertexComponentType::Uint8>(
    const uint8_t* data,
    size_t numVertices,
    uint32_t numComponents)
{
    using DstType = uint16_t;
    using SrcType = uint8_t;

    const auto totalNumComponents = numVertices * numComponents;
    auto dstData = new DstType[totalNumComponents];
    auto srcData = reinterpret_cast<const SrcType*>(data);
    for (size_t i = 0u; i < totalNumComponents; ++i)
        dstData[i] = srcData[i];
    return reinterpret_cast<uint8_t*>(dstData);
}

template<>
inline uint8_t* convertToVertexComponentType<VertexComponentType::Uint32, VertexComponentType::Uint8>(
    const uint8_t* data,
    size_t numVertices,
    uint32_t numComponents)
{
    using DstType = uint32_t;
    using SrcType = uint8_t;

    const auto totalNumComponents = numVertices * numComponents;
    auto dstData = new DstType[totalNumComponents];
    auto srcData = reinterpret_cast<const SrcType*>(data);
    for (size_t i = 0u; i < totalNumComponents; ++i)
        dstData[i] = srcData[i];
    return reinterpret_cast<uint8_t*>(dstData);
}

template<>
inline uint8_t* convertToVertexComponentType<VertexComponentType::Uint32, VertexComponentType::Uint16>(
    const uint8_t* data,
    size_t numVertices,
    uint32_t numComponents)
{
    using DstType = uint32_t;
    using SrcType = uint16_t;

    const auto totalNumComponents = numVertices * numComponents;
    auto dstData = new DstType[totalNumComponents];
    auto srcData = reinterpret_cast<const SrcType*>(data);
    for (size_t i = 0u; i < totalNumComponents; ++i)
        dstData[i] = srcData[i];
    return reinterpret_cast<uint8_t*>(dstData);
}

template<VertexComponentType DstType>
inline uint8_t* convertToVertexComponentType(
    const uint8_t* data,
    size_t numVertices,
    uint32_t numComponents,
    VertexComponentType type)
{
    uint8_t* result = nullptr;

    switch (type)
    {
    case VertexComponentType::Single:
    {
        result = convertToVertexComponentType<DstType, VertexComponentType::Single>(data, numVertices, numComponents);
        break;
    }
    case VertexComponentType::Double:
    {
        result = convertToVertexComponentType<DstType, VertexComponentType::Double>(data, numVertices, numComponents);
        break;
    }
    case VertexComponentType::Int8:
    {
        result = convertToVertexComponentType<DstType, VertexComponentType::Int8>(data, numVertices, numComponents);
        break;
    }
    case VertexComponentType::Uint8:
    {
        result = convertToVertexComponentType<DstType, VertexComponentType::Uint8>(data, numVertices, numComponents);
        break;
    }
    case VertexComponentType::Int16:
    {
        result = convertToVertexComponentType<DstType, VertexComponentType::Int16>(data, numVertices, numComponents);
        break;
    }
    case VertexComponentType::Uint16:
    {
        result = convertToVertexComponentType<DstType, VertexComponentType::Uint16>(data, numVertices, numComponents);
        break;
    }
    case VertexComponentType::Int32:
    {
        result = convertToVertexComponentType<DstType, VertexComponentType::Int32>(data, numVertices, numComponents);
        break;
    }
    case VertexComponentType::Uint32:
    {
        result = convertToVertexComponentType<DstType, VertexComponentType::Uint32>(data, numVertices, numComponents);
        break;
    }
    default:
    {
        LOG_CRITICAL << "Undefined source type";
        break;
    }
    }

    return result;
}

static uint8_t* convertToVertexComponentType(
    const uint8_t* data,
    size_t numVertices,
    uint32_t numComponents,
    VertexComponentType& type,
    VertexComponentType newType)
{
    uint8_t* result = nullptr;

    switch (newType)
    {
    case VertexComponentType::Single:
    {
        result = convertToVertexComponentType<VertexComponentType::Single>(data, numVertices, numComponents, type);
        break;
    }
    case VertexComponentType::Double:
    {
        result = convertToVertexComponentType<VertexComponentType::Double>(data, numVertices, numComponents, type);
        break;
    }
    case VertexComponentType::Int8:
    {
        result = convertToVertexComponentType<VertexComponentType::Int8>(data, numVertices, numComponents, type);
        break;
    }
    case VertexComponentType::Uint8:
    {
        result = convertToVertexComponentType<VertexComponentType::Uint8>(data, numVertices, numComponents, type);
        break;
    }
    case VertexComponentType::Int16:
    {
        result = convertToVertexComponentType<VertexComponentType::Int16>(data, numVertices, numComponents, type);
        break;
    }
    case VertexComponentType::Uint16:
    {
        result = convertToVertexComponentType<VertexComponentType::Uint16>(data, numVertices, numComponents, type);
        break;
    }
    case VertexComponentType::Int32:
    {
        result = convertToVertexComponentType<VertexComponentType::Int32>(data, numVertices, numComponents, type);
        break;
    }
    case VertexComponentType::Uint32:
    {
        result = convertToVertexComponentType<VertexComponentType::Uint32>(data, numVertices, numComponents, type);
        break;
    }
    default:
    {
        LOG_CRITICAL << "Undefined destination type";
        break;
    }
    }

    type = newType;
    return result;
}


template <DrawElementsIndexType DstType, DrawElementsIndexType SrcType>
inline uint8_t* convertToDrawElementsIndexType(
    const uint8_t* data,
    size_t numIndices)
{
    if constexpr (DstType != SrcType)
        LOG_CRITICAL << "Decrease the size of index type is not allowed";
    return nullptr;
}

template<>
inline uint8_t* convertToDrawElementsIndexType<DrawElementsIndexType::Uint16, DrawElementsIndexType::Uint8>(
    const uint8_t* data,
    size_t numIndices)
{
    using DstType = uint16_t;
    using SrcType = uint8_t;

    auto dstData = new DstType[numIndices];
    auto srcData = reinterpret_cast<const SrcType*>(data);
    for (size_t i = 0u; i < numIndices; ++i)
        dstData[i] = srcData[i];
    return reinterpret_cast<uint8_t*>(dstData);
}

template<>
inline uint8_t* convertToDrawElementsIndexType<DrawElementsIndexType::Uint32, DrawElementsIndexType::Uint8>(
    const uint8_t* data,
    size_t numIndices)
{
    using DstType = uint32_t;
    using SrcType = uint8_t;

    auto dstData = new DstType[numIndices];
    auto srcData = reinterpret_cast<const SrcType*>(data);
    for (size_t i = 0u; i < numIndices; ++i)
        dstData[i] = srcData[i];
    return reinterpret_cast<uint8_t*>(dstData);
}

template<>
inline uint8_t* convertToDrawElementsIndexType<DrawElementsIndexType::Uint32, DrawElementsIndexType::Uint16>(
    const uint8_t* data,
    size_t numIndices)
{
    using DstType = uint32_t;
    using SrcType = uint16_t;

    auto dstData = new DstType[numIndices];
    auto srcData = reinterpret_cast<const SrcType*>(data);
    for (size_t i = 0u; i < numIndices; ++i)
        dstData[i] = srcData[i];
    return reinterpret_cast<uint8_t*>(dstData);
}

template <DrawElementsIndexType DstType>
inline uint8_t* convertToDrawElementsIndexType(
    const uint8_t* data,
    size_t numIndices,
    DrawElementsIndexType indexType)
{
    uint8_t* result = nullptr;

    switch (indexType)
    {
    case DrawElementsIndexType::Uint8:
    {
        result = convertToDrawElementsIndexType<DstType, DrawElementsIndexType::Uint8>(data, numIndices);
        break;
    }
    case DrawElementsIndexType::Uint16:
    {
        result = convertToDrawElementsIndexType<DstType, DrawElementsIndexType::Uint16>(data, numIndices);
        break;
    }
    case DrawElementsIndexType::Uint32:
    {
        result = convertToDrawElementsIndexType<DstType, DrawElementsIndexType::Uint32>(data, numIndices);
        break;
    }
    default:
    {
        LOG_CRITICAL << "Undefined source type";
        break;
    }
    }

    return result;
}

static uint8_t* convertToDrawElementsIndexType(
    const uint8_t* data,
    size_t numIndices,
    DrawElementsIndexType& indexType,
    DrawElementsIndexType newIndexType)
{
    uint8_t* result = nullptr;

    switch (newIndexType)
    {
    case DrawElementsIndexType::Uint8:
    {
        result = convertToDrawElementsIndexType<DrawElementsIndexType::Uint8>(data, numIndices, indexType);
        break;
    }
    case DrawElementsIndexType::Uint16:
    {
        result = convertToDrawElementsIndexType<DrawElementsIndexType::Uint16>(data, numIndices, indexType);
        break;
    }
    case DrawElementsIndexType::Uint32:
    {
        result = convertToDrawElementsIndexType<DrawElementsIndexType::Uint32>(data, numIndices, indexType);
        break;
    }
    default:
    {
        LOG_CRITICAL << "Undefined destination type";
        break;
    }
    }

    indexType = newIndexType;
    return result;
}

template<typename T>
inline uint8_t* convertPointsToTriangles(
    const void* data,
    size_t& numIndices)
{
    numIndices = numIndices * 3u;
    auto result = new T[numIndices];
    for (size_t i = 0u; i < numIndices; ++i)
    {
        result[3u * i + 0u] = static_cast<T>(i);
        result[3u * i + 1u] = static_cast<T>(i);
        result[3u * i + 2u] = static_cast<T>(i);
    }
    return reinterpret_cast<uint8_t*>(result);
}

inline uint8_t* convertPointsToTriangles(
    const uint8_t* data,
    size_t& numIndices,
    DrawElementsIndexType indexType)
{
    uint8_t* result = nullptr;

    switch (indexType)
    {
    case DrawElementsIndexType::Uint8:
    {
        result = convertPointsToTriangles<uint8_t>(data, numIndices);
        break;
    }
    case DrawElementsIndexType::Uint16:
    {
        result = convertPointsToTriangles<uint16_t>(data, numIndices);
        break;
    }
    case DrawElementsIndexType::Uint32:
    {
        result = convertPointsToTriangles<uint32_t>(data, numIndices);
        break;
    }
    default:
    {
        LOG_CRITICAL << "Undefined index type";
        break;
    }
    }

    return result;
}

template<typename T>
inline uint8_t* convertLinesToTriangles(
    const void* data,
    size_t& numIndices)
{
    const auto numLines = numIndices / 2u;
    numIndices = numLines * 3u;
    auto result = new T[numIndices];
    for (size_t i = 0u; i < numLines; ++i)
    {
        result[3u * i + 0u] = static_cast<T>(2u * i + 0u);
        result[3u * i + 1u] = static_cast<T>(2u * i + 0u);
        result[3u * i + 2u] = static_cast<T>(2u * i + 1u);
    }
    return reinterpret_cast<uint8_t*>(result);
}

inline uint8_t* convertLinesToTriangles(
    const uint8_t* data,
    size_t& numIndices,
    DrawElementsIndexType indexType)
{
    uint8_t* result = nullptr;

    switch (indexType)
    {
    case DrawElementsIndexType::Uint8:
    {
        result = convertLinesToTriangles<uint8_t>(data, numIndices);
        break;
    }
    case DrawElementsIndexType::Uint16:
    {
        result = convertLinesToTriangles<uint16_t>(data, numIndices);
        break;
    }
    case DrawElementsIndexType::Uint32:
    {
        result = convertLinesToTriangles<uint32_t>(data, numIndices);
        break;
    }
    default:
    {
        LOG_CRITICAL << "Undefined index type";
        break;
    }
    }

    return result;
}

template<typename T>
inline uint8_t* convertLineStripToTriangles(
    const void* data,
    size_t& numIndices)
{
    if (numIndices < 2u)
        return nullptr;
    const auto numLines = (numIndices - 1u);
    numIndices = numLines * 3u;
    auto result = new T[numIndices];
    for (size_t i = 0u; i < numLines; ++i)
    {
        result[3u * i + 0u] = static_cast<T>(i);
        result[3u * i + 1u] = static_cast<T>(i);
        result[3u * i + 2u] = static_cast<T>(i + 1u);
    }
    return reinterpret_cast<uint8_t*>(result);
}

inline uint8_t* convertLineStripToTriangles(
    const uint8_t* data,
    size_t& numIndices,
    DrawElementsIndexType indexType)
{
    uint8_t* result = nullptr;

    switch (indexType)
    {
    case DrawElementsIndexType::Uint8:
    {
        result = convertLineStripToTriangles<uint8_t>(data, numIndices);
        break;
    }
    case DrawElementsIndexType::Uint16:
    {
        result = convertLineStripToTriangles<uint16_t>(data, numIndices);
        break;
    }
    case DrawElementsIndexType::Uint32:
    {
        result = convertLineStripToTriangles<uint32_t>(data, numIndices);
        break;
    }
    default:
    {
        LOG_CRITICAL << "Undefined index type";
        break;
    }
    }

    return result;
}

template<typename T>
inline uint8_t* convertTriangleStripToTriangles(
    const void* data,
    size_t& numIndices)
{
    if (numIndices < 3u)
        return nullptr;
    const auto numTriangles = (numIndices - 2u);
    numIndices = numTriangles * 3u;
    auto result = new T[numIndices];
    for (size_t i = 0u; i < numTriangles; ++i)
    {
        result[3u * i + 0u] = static_cast<T>(i + (i % 2u ? 1u : 0u));
        result[3u * i + 1u] = static_cast<T>(i + (i % 2u ? 0u : 1u));
        result[3u * i + 2u] = static_cast<T>(i + 2u);
    }
    return reinterpret_cast<uint8_t*>(result);
}

inline uint8_t* convertTriangleStripToTriangles(
    const uint8_t* data,
    size_t& numIndices,
    DrawElementsIndexType indexType)
{
    uint8_t* result = nullptr;

    switch (indexType)
    {
    case DrawElementsIndexType::Uint8:
    {
        result = convertTriangleStripToTriangles<uint8_t>(data, numIndices);
        break;
    }
    case DrawElementsIndexType::Uint16:
    {
        result = convertTriangleStripToTriangles<uint16_t>(data, numIndices);
        break;
    }
    case DrawElementsIndexType::Uint32:
    {
        result = convertTriangleStripToTriangles<uint32_t>(data, numIndices);
        break;
    }
    default:
    {
        LOG_CRITICAL << "Undefined index type";
        break;
    }
    }

    return result;
}

template<typename T>
inline uint8_t* convertTriangleFanToTriangles(
    const void* data,
    size_t& numIndices)
{
    if (numIndices < 3u)
        return nullptr;
    const auto numTriangles = (numIndices - 2u);
    numIndices = numTriangles * 3u;
    auto result = new T[numIndices];
    for (size_t i = 0u; i < numTriangles; ++i)
    {
        result[3u * i + 0u] = static_cast<T>(0u);
        result[3u * i + 1u] = static_cast<T>(i + 1u);
        result[3u * i + 2u] = static_cast<T>(i + 2u);
    }
    return reinterpret_cast<uint8_t*>(result);
}

inline uint8_t* convertTriangleFanToTriangles(
    const uint8_t* data,
    size_t& numIndices,
    DrawElementsIndexType indexType)
{
    uint8_t* result = nullptr;

    switch (indexType)
    {
    case DrawElementsIndexType::Uint8:
    {
        result = convertTriangleFanToTriangles<uint8_t>(data, numIndices);
        break;
    }
    case DrawElementsIndexType::Uint16:
    {
        result = convertTriangleFanToTriangles<uint16_t>(data, numIndices);
        break;
    }
    case DrawElementsIndexType::Uint32:
    {
        result = convertTriangleFanToTriangles<uint32_t>(data, numIndices);
        break;
    }
    default:
    {
        LOG_CRITICAL << "Undefined index type";
        break;
    }
    }

    return result;
}

static uint8_t* convertToDrawElementsTriangles(
    const uint8_t* data,
    size_t& numIndices,
    DrawElementsIndexType indexType,
    PrimitiveType& primitiveType)
{
    uint8_t* result = nullptr;

    switch (primitiveType)
    {
    case PrimitiveType::Points:
    {
        result = convertPointsToTriangles(data, numIndices, indexType);
        break;
    }
    case PrimitiveType::Lines:
    {
        result = convertLinesToTriangles(data, numIndices, indexType);
        break;
    }
    case PrimitiveType::LineStrip:
    {
        result = convertLineStripToTriangles(data, numIndices, indexType);
        break;
    }
    case PrimitiveType::Triangles:
    {
        // do nothing
        break;
    }
    case PrimitiveType::TriangleStrip:
    {
        result = convertTriangleStripToTriangles(data, numIndices, indexType);
        break;
    }
    case PrimitiveType::TriangleFan:
    {
        result = convertTriangleFanToTriangles(data, numIndices, indexType);
        break;
    }

    default:
        LOG_CRITICAL << "Undefined primitive type";
        break;
    }

    if (result)
        primitiveType = PrimitiveType::Triangles;

    return result;
}

template<typename T>
inline uint8_t* applyDrawElementsBaseVertex(
    const uint8_t* data,
    size_t numIndices,
    size_t baseVertex)
{
    auto result = new T[numIndices];
    for (size_t i = 0u; i < numIndices; ++i)
    {
        result[i] = reinterpret_cast<const T*>(data)[i] + baseVertex;
    }

    return reinterpret_cast<uint8_t*>(result);
}

static uint8_t* applyDrawElementsBaseVertex(
    const uint8_t* data,
    size_t numIndices,
    DrawElementsIndexType indexType,
    size_t& baseVertex)
{
    uint8_t* result = nullptr;

    switch (indexType)
    {
    case DrawElementsIndexType::Uint8:
    {
        result = applyDrawElementsBaseVertex<uint8_t>(data, numIndices, baseVertex);
        break;
    }
    case DrawElementsIndexType::Uint16:
    {
        result = applyDrawElementsBaseVertex<uint16_t>(data, numIndices, baseVertex);
        break;
    }
    case DrawElementsIndexType::Uint32:
    {
        result = applyDrawElementsBaseVertex<uint32_t>(data, numIndices, baseVertex);
        break;
    }
    default:
    {
        LOG_CRITICAL << "Undefined index type";
        break;
    }
    }

    baseVertex = 0u;
    return result;
}

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
    if (drawArrays->primitiveType() != PrimitiveType::Triangles)
        LOG_CRITICAL << "Primitive type must be Triangle";

    if (verticesNumComponents < 3u)
        LOG_CRITICAL << "Num components must be greater or equal than 3";

    if (normalsNumComponents < 3u)
        LOG_CRITICAL << "Num components must be greater or equal than 3";

    const auto drawArraysFirst = drawArrays->first();
    const auto drawArraysCount = drawArrays->count();

    for (size_t i = 0u; i < drawArraysCount; i += 3)
    {
        const size_t i0 = i + drawArraysFirst + 0u;
        const size_t i1 = i + drawArraysFirst + 1u;
        const size_t i2 = i + drawArraysFirst + 2u;
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
    if (drawElements->primitiveType() != PrimitiveType::Triangles)
        LOG_CRITICAL << "Primitive type must be Triangle";

    if (verticesNumComponents < 3u)
        LOG_CRITICAL << "Num components must be greater or equal than 3";

    if (normalsNumComponents < 3u)
        LOG_CRITICAL << "Num components must be greater or equal than 3";

    const auto drawElementsCount = drawElements->count();
    const auto drawElementsBaseVertex = drawElements->baseVertex();
    const auto *pIndices = reinterpret_cast<const I*>(reinterpret_cast<const uint8_t*>(indices) + drawElements->offset());

    for (size_t i = 0u; i < drawElementsCount; i += 3)
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
inline void normalize(size_t numVertices,
                      uint32_t normalsNumComponents,
                      const V *normals,
                      uint32_t resultNumComponents,
                      V *result)

{
    if (normalsNumComponents < 3u)
        LOG_CRITICAL << "Num components must be greater or equal than 3";

    if (resultNumComponents < 3u)
        LOG_CRITICAL << "Num components must be greater or equal than 3";

    for (size_t i = 0u; i < numVertices; ++i)
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
    if (drawArrays->primitiveType() != PrimitiveType::Triangles)
        LOG_CRITICAL << "Primitive type must be Triangle";

    if (verticesNumComponents < 3u)
        LOG_CRITICAL << "Num components must be greater or equal than 3";

    if (texCoordsNumComponents < 2u)
        LOG_CRITICAL << "Num components must be greater or equal than 2";

    if (tangentsNumComponents < 3u)
        LOG_CRITICAL << "Num components must be greater or equal than 3";

    if (binormalsNumComponents < 3u)
        LOG_CRITICAL << "Num components must be greater or equal than 3";

    const auto drawArraysFirst = drawArrays->first();
    const auto drawArraysCount = drawArrays->count();

    for (size_t i = 0u; i < drawArraysCount; i += 3)
    {
        const size_t i0 = i + drawArraysFirst + 0u;
        const size_t i1 = i + drawArraysFirst + 1u;
        const size_t i2 = i + drawArraysFirst + 2u;
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
    if (drawElements->primitiveType() != PrimitiveType::Triangles)
        LOG_CRITICAL << "Primitive type must be Triangle";

    if (verticesNumComponents < 3u)
        LOG_CRITICAL << "Num components must be greater or equal than 3";

    if (texCoordsNumComponents < 2u)
        LOG_CRITICAL << "Num components must be greater or equal than 2";

    if (tangentsNumComponents < 3u)
        LOG_CRITICAL << "Num components must be greater or equal than 3";

    if (binormalsNumComponents < 3u)
        LOG_CRITICAL << "Num components must be greater or equal than 3";

    const auto drawElementsCount = drawElements->count();
    const auto drawElementsBaseVertex = drawElements->baseVertex();
    const auto *pIndices = reinterpret_cast<const I*>(reinterpret_cast<const uint8_t*>(indices) + drawElements->offset());

    for (size_t i = 0u; i < drawElementsCount; i += 3)
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
inline void orthogonalizeTangentSpace(size_t numVertices,
                                      uint32_t tangentsNumComponents,
                                      const V *tangents,
                                      uint32_t binormalsNumComponents,
                                      const V *binormals,
                                      uint32_t normalsNumComponents,
                                      const V *normals,
                                      uint32_t resultNumComponents,
                                      V *result)

{
    if (tangentsNumComponents < 3u)
        LOG_CRITICAL << "Num components must be greater or equal than 3";

    if (binormalsNumComponents < 3u)
        LOG_CRITICAL << "Num components must be greater or equal than 2";

    if (normalsNumComponents < 3u)
        LOG_CRITICAL << "Num components must be greater or equal than 3";

    if (resultNumComponents < 4u)
        LOG_CRITICAL << "Num components must be greater or equal than 4";

    for (size_t i = 0u; i < numVertices; ++i)
    {
        const auto &n = *reinterpret_cast<const glm::vec<3u, V>*>(normals + i * normalsNumComponents);
        const auto &t = *reinterpret_cast<const glm::vec<3u, V>*>(tangents + i * tangentsNumComponents);
        const auto &b = *reinterpret_cast<const glm::vec<3u, V>*>(binormals + i * binormalsNumComponents);

        glm::vec4 q = glm::vec<4u, V>(glm::normalize(t - n * glm::dot(t, n)),
                                      (glm::dot(glm::cross(n, t), b) < static_cast<V>(0)) ? static_cast<V>(-1) : static_cast<V>(+1));

        *reinterpret_cast<glm::vec<4u, V>*>(result + i * resultNumComponents) = q;
    }

}

template<typename V>
inline BoundingBox calculateBoundingBox(const std::shared_ptr<DrawArrays> &drawArrays,
                                        uint32_t verticesNumComponents,
                                        const V *vertices)
{
    if (verticesNumComponents < 1u)
        LOG_CRITICAL << "Num components must be greater or equal than 1";

    BoundingBox result;

    const auto drawArraysFirst = drawArrays->first();
    const auto drawArraysCount = drawArrays->count();

    verticesNumComponents = glm::min(verticesNumComponents, static_cast<uint32_t>(BoundingBox::length()));

    for (size_t i = 0u; i < drawArraysCount; ++i)
    {
        const auto p = reinterpret_cast<const V*>(vertices + (i + drawArraysFirst) * verticesNumComponents);
        BoundingBox::PointType v(0.f);
        for (uint32_t j = 0u; j < verticesNumComponents; ++j)
            v[j] = p[j];
        result += v;
    }

    return result;
}

template<typename V, typename I>
inline BoundingBox calculateBoundingBox(const std::shared_ptr<DrawElements> &drawElements,
                                        uint32_t verticesNumComponents,
                                        const V *vertices,
                                        const I *indices)
{
    if (verticesNumComponents < 1u)
        LOG_CRITICAL << "Num components must be greater or equal than 1";

    BoundingBox result;

    const auto drawElementsCount = drawElements->count();
    const auto drawElementsBaseVertex = drawElements->baseVertex();
    const auto *pIndices = reinterpret_cast<const I*>(reinterpret_cast<const uint8_t*>(indices) + drawElements->offset());

    verticesNumComponents = glm::min(verticesNumComponents, static_cast<uint32_t>(BoundingBox::length()));

    for (size_t i = 0u; i < drawElementsCount; ++i)
    {
        const I index = static_cast<I>(pIndices[i] + drawElementsBaseVertex);
        const auto p = reinterpret_cast<const V*>(vertices + index * verticesNumComponents);
        BoundingBox::PointType v(0.f);
        for (uint32_t j = 0u; j < verticesNumComponents; ++j)
            v[j] = p[j];
        result += v;
    }

    return result;
}

}
}

#endif // UTILS_MESHIMPL_H

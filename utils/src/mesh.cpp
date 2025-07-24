#include <cstring>
#include <algorithm>
#include <array>

#include <utils/logger.h>
#include <utils/mesh.h>

#include "meshimpl.h"

namespace simplex
{
namespace utils
{

size_t sizeOfVertexComponentType(VertexComponentType type)
{
    static std::array<uint32_t, numElementsVertexComponentType()> s_table{
        sizeof(float),
        sizeof(double),
        sizeof(int8_t),
        sizeof(uint8_t),
        sizeof(int16_t),
        sizeof(uint16_t),
        sizeof(int32_t),
        sizeof(uint32_t)
    };

    return s_table[utils::castFromVertexComponentType(type)];
}

Buffer::Buffer(size_t sizeInBytes)
    : m_data(nullptr)
    , m_sizeInBytes(sizeInBytes)
{
    if (m_sizeInBytes)
        m_data = new uint8_t[m_sizeInBytes];
}

Buffer::~Buffer()
{
    delete [] m_data;
}

size_t Buffer::sizeInBytes() const
{
    return m_sizeInBytes;
}

void Buffer::resize(size_t sizeInBytes)
{
    if (m_sizeInBytes == sizeInBytes)
        return;

    auto newData = new uint8_t[sizeInBytes];
    if (m_data)
        std::memcpy(newData, m_data, std::min(sizeInBytes, m_sizeInBytes));

    delete [] m_data;
    m_data = newData;
    m_sizeInBytes = sizeInBytes;
}

uint8_t *Buffer::data()
{
    return m_data;
}

const uint8_t *Buffer::data() const
{
    return m_data;
}

VertexBuffer::VertexBuffer(size_t numVertices, uint32_t numComponents, VertexComponentType type)
    : Buffer(0u)
    , m_numComponents(numComponents)
    , m_type(type)
{
    setNumVertices(numVertices);
}

VertexBuffer::~VertexBuffer() = default;

uint32_t VertexBuffer::numComponents() const
{
    return m_numComponents;
}

size_t VertexBuffer::numVertices() const
{
    const size_t vertexSize = m_numComponents * sizeOfVertexComponentType(m_type);
    return static_cast<uint32_t>(m_sizeInBytes / vertexSize);
}

void VertexBuffer::setNumVertices(size_t numVertices)
{
    resize(numVertices * m_numComponents * sizeOfVertexComponentType(m_type));
}

VertexComponentType VertexBuffer::componentType() const
{
    return m_type;
}

const uint8_t *VertexBuffer::vertex(size_t index) const
{
    return m_data + m_numComponents * sizeOfVertexComponentType(m_type) * index;
}

void VertexBuffer::setVertex(size_t index, const uint8_t*data)
{
    std::memcpy(m_data + m_numComponents * sizeOfVertexComponentType(m_type) * index,
                data,
                m_numComponents * sizeOfVertexComponentType(m_type));
}

std::shared_ptr<VertexBuffer> VertexBuffer::copy() const
{
    auto result = std::make_shared<VertexBuffer>(numVertices(), numComponents(), componentType());
    std::memcpy(result->m_data, m_data, m_sizeInBytes);
    return result;
}

void VertexBuffer::convert(uint32_t numComponents, VertexComponentType type)
{
    if (m_numComponents != numComponents)
    {
        const auto sizeOfType = sizeOfVertexComponentType(m_type);

        auto data = new uint8_t[numVertices() * numComponents * sizeOfType];
        std::fill(data, data + numVertices() * numComponents * sizeOfType, 0u);

        auto minNumComponents = std::min(numComponents, m_numComponents);

        for (size_t i = 0u; i < numVertices(); ++i)
        {
            std::memcpy(
                data + i * numComponents * sizeOfType,
                m_data + i * m_numComponents * sizeOfType,
                minNumComponents);
        }

        delete[] m_data;
        m_data = data;

        m_numComponents = numComponents;
    }

    if (m_type != type)
    {
        if (auto data = convertToVertexComponentType(m_data, numVertices(), m_numComponents, m_type, type))
        {
            delete[] m_data;
            m_data = data;
        }
    }
}

std::shared_ptr<VertexBuffer> VertexBuffer::converted(uint32_t numComponents, VertexComponentType type) const
{
    auto result = copy();
    result->convert(numComponents, type);
    return result;
}

DrawElementsBuffer::DrawElementsBuffer(PrimitiveType primitiveType, uint32_t count, DrawElementsIndexType type, size_t baseVertex)
    : DrawElements(primitiveType, count, type, 0, baseVertex)
    , Buffer(0u)
{
    if (m_indexType == DrawElementsIndexType::Count)
        LOG_CRITICAL << "Undefined draw elements index type";

    setNumIndices(m_count);
}

DrawElementsBuffer::~DrawElementsBuffer() = default;

std::shared_ptr<DrawElementsBuffer> DrawElementsBuffer::asDrawElementsBuffer()
{
    return std::dynamic_pointer_cast<DrawElementsBuffer>(shared_from_this());
}

std::shared_ptr<const DrawElementsBuffer> DrawElementsBuffer::asDrawElementsBuffer() const
{
    return std::dynamic_pointer_cast<const DrawElementsBuffer>(shared_from_this());
}

size_t DrawElementsBuffer::numIndices() const
{
    return count();
}

void DrawElementsBuffer::setNumIndices(size_t numIndices)
{
    m_count = numIndices;
    resize(m_count * indexSize());
}

const void *DrawElementsBuffer::index(size_t idx) const
{
    return static_cast<const void*>(m_data + m_offset + indexSize() * idx);
}

void DrawElementsBuffer::setIndex(size_t idx, const void *data)
{
    std::memcpy(m_data + m_offset + indexSize() * idx, data, indexSize());
}

std::shared_ptr<DrawElementsBuffer> DrawElementsBuffer::copy() const
{
    auto result = std::make_shared<DrawElementsBuffer>(primitiveType(), numIndices(), indexType(), baseVertex());
    std::memcpy(result->m_data, m_data, m_sizeInBytes);
    return result;
}

void DrawElementsBuffer::convertToIndexType(DrawElementsIndexType indexType)
{
    if (m_indexType != indexType)
    {
        if (auto data = convertToDrawElementsIndexType(
            m_data + m_offset * indexSize(),
            numIndices(),
            m_indexType,
            indexType))
        {
            delete[] m_data;
            m_data = data;
            m_sizeInBytes = numIndices() * indexSize();
        }
    }
}

std::shared_ptr<DrawElementsBuffer> DrawElementsBuffer::convertedToIndexType(DrawElementsIndexType indexType) const
{
    auto result = copy();
    result->convertToIndexType(indexType);
    return result;
}

void DrawElementsBuffer::convertToTriangles()
{
    if (m_primitiveType != PrimitiveType::Triangles)
    {
        if (auto data = convertToDrawElementsTriangles(
            m_data + m_offset * indexSize(),
            m_count,
            indexType(),
            m_primitiveType))
        {
            delete[] m_data;
            m_data = data;
            m_sizeInBytes = numIndices() * indexSize();
        }
    }
}

std::shared_ptr<DrawElementsBuffer> DrawElementsBuffer::convertedToTriangles() const
{
    auto result = copy();
    result->convertToTriangles();
    return result;
}

void DrawElementsBuffer::applyBaseVertex()
{
    if (m_baseVertex)
    {
        if (auto data = applyDrawElementsBaseVertex(
            m_data + m_offset * indexSize(),
            m_count,
            indexType(),
            m_baseVertex))
        {
            delete[] m_data;
            m_data = data;
            m_sizeInBytes = numIndices() * indexSize();
        }
    }
}

std::shared_ptr<DrawElementsBuffer> DrawElementsBuffer::appliedBaseVertex() const
{
    auto result = copy();
    result->applyBaseVertex();
    return result;
}

Mesh::Mesh()
{
}

void Mesh::attachVertexBuffer(VertexAttribute vertexAttribute, const std::shared_ptr<VertexBuffer> &vertexBuffer)
{
    if (!vertexBuffer)
        LOG_CRITICAL << "Vertex buffer can't be nullptr";

    m_vertexBuffers[vertexAttribute] = vertexBuffer;
}

void Mesh::detachVertexBuffer(VertexAttribute vertexAttribute)
{
    m_vertexBuffers.erase(vertexAttribute);
}

const std::unordered_map<VertexAttribute, std::shared_ptr<VertexBuffer>> &Mesh::vertexBuffers() const
{
    return m_vertexBuffers;
}

void Mesh::attachPrimitiveSet(const std::shared_ptr<DrawArrays> &primitiveSet)
{
    if (!primitiveSet)
        LOG_CRITICAL << "Primitive set can't be nullptr";

    m_primitiveSets.insert(primitiveSet);
}

void Mesh::attachPrimitiveSet(const std::shared_ptr<DrawElementsBuffer> &primitiveSet)
{
    if (!primitiveSet)
        LOG_CRITICAL << "Primitive set can't be nullptr";

    m_primitiveSets.insert(primitiveSet);
}

void Mesh::detachPrimitiveSet(const std::shared_ptr<PrimitiveSet> &primitiveSet)
{
    m_primitiveSets.erase(primitiveSet);
}

const std::unordered_set<std::shared_ptr<PrimitiveSet>> &Mesh::primitiveSets() const
{
    return m_primitiveSets;
}

std::shared_ptr<Mesh> Mesh::createEmptyMesh(const std::unordered_map<VertexAttribute, std::tuple<uint32_t, VertexComponentType>> &decls)
{
    auto result = std::make_shared<Mesh>();
    for (const auto &[attrib, decl] : decls)
        result->attachVertexBuffer(attrib, std::make_shared<VertexBuffer>(0u, std::get<0>(decl), std::get<1>(decl)));
    return result;
}

}
}

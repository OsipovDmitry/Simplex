#include <cstring>
#include <cassert>
#include <algorithm>

#include <utils/mesh.h>
#include <utils/typeinfo.h>

namespace simplex
{
namespace utils
{

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

VertexBuffer::VertexBuffer(uint32_t numVertices, uint32_t numComponents, Type type)
    : Buffer(numVertices * numComponents * TypeInfo::size(type))
    , m_numComponents(numComponents)
    , m_type(type)
{
    assert(m_numComponents <= 4u);
}

VertexBuffer::~VertexBuffer()
{
}

uint32_t VertexBuffer::numComponents() const
{
    return m_numComponents;
}

uint32_t VertexBuffer::numVertices() const
{
    const uint32_t vertexSize = m_numComponents * TypeInfo::size(m_type);
    return static_cast<uint32_t>(m_sizeInBytes / vertexSize);
}

void VertexBuffer::setNumVertices(uint32_t numVertices)
{
    resize(numVertices * m_numComponents * TypeInfo::size(m_type));
}

Type VertexBuffer::type() const
{
    return m_type;
}

const void *VertexBuffer::vertex(uint32_t index) const
{
    return static_cast<const void*>(m_data + m_numComponents * TypeInfo::size(m_type) * index);
}

void VertexBuffer::setVertex(uint32_t index, const void *data)
{
    std::memcpy(m_data + m_numComponents * TypeInfo::size(m_type) * index, data, m_numComponents * TypeInfo::size(m_type));
}

DrawElementsBuffer::DrawElementsBuffer(PrimitiveType primitiveType, uint32_t count, Type type, uint32_t baseVertex)
    : DrawElements(primitiveType, count, type, 0, baseVertex)
    , Buffer(count * TypeInfo::size(type))
{
}

DrawElementsBuffer::~DrawElementsBuffer()
{
}

std::shared_ptr<DrawElementsBuffer> DrawElementsBuffer::asDrawElementsBuffer()
{
    return std::dynamic_pointer_cast<DrawElementsBuffer>(shared_from_this());
}

std::shared_ptr<const DrawElementsBuffer> DrawElementsBuffer::asDrawElementsBuffer() const
{
    return std::dynamic_pointer_cast<const DrawElementsBuffer>(shared_from_this());
}

uint32_t DrawElementsBuffer::numIndices() const
{
    return count();
}

void DrawElementsBuffer::setNumIndices(uint32_t numIndices)
{
    m_count = numIndices;
    resize(numIndices * TypeInfo::size(m_type));
}

const void *DrawElementsBuffer::index(uint32_t idx) const
{
    return static_cast<const void*>(m_data + TypeInfo::size(m_type) * idx);
}

void DrawElementsBuffer::setIndex(uint32_t idx, const void *data)
{
    std::memcpy(m_data + TypeInfo::size(m_type) * idx, data, TypeInfo::size(m_type));
}

Mesh::Mesh()
{
}

void Mesh::attachVertexBuffer(VertexAttribute vertexAttribute, std::shared_ptr<VertexBuffer> vertexBuffer)
{
    assert(vertexBuffer);
    m_vertexBuffers.insert({vertexAttribute, vertexBuffer});
}

void Mesh::detachVertexBuffer(VertexAttribute vertexAttribute)
{
    m_vertexBuffers.erase(vertexAttribute);
}

const std::unordered_map<VertexAttribute, std::shared_ptr<VertexBuffer> > &Mesh::vertexBuffers() const
{
    return m_vertexBuffers;
}

void Mesh::attachPrimitiveSet(std::shared_ptr<DrawArrays> primitiveSet)
{
    assert(primitiveSet);
    m_primitiveSets.insert(primitiveSet);
}

void Mesh::attachPrimitiveSet(std::shared_ptr<DrawElementsBuffer> primitiveSet)
{
    assert(primitiveSet);
    m_primitiveSets.insert(primitiveSet);
}

void Mesh::detachPrimitiveSet(std::shared_ptr<PrimitiveSet> primitiveSet)
{
    m_primitiveSets.erase(primitiveSet);
}

const std::unordered_set<std::shared_ptr<PrimitiveSet> > &Mesh::primitiveSets() const
{
    return m_primitiveSets;
}

}
}

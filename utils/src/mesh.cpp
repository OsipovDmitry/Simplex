#include <cstring>
#include <cassert>
#include <algorithm>

#include <utils/mesh.h>
#include <utils/typeinfo.h>

namespace simplex
{
namespace utils
{

Buffer::Buffer(uint64_t sizeInBytes)
    : m_data(nullptr)
    , m_sizeInBytes(sizeInBytes)
{
    if (m_sizeInBytes)
        m_data = new uint8_t[m_sizeInBytes];
}

Buffer::~Buffer()
{
    clear();
}

uint64_t Buffer::sizeInBytes() const
{
    return m_sizeInBytes;
}

void Buffer::clear()
{
    delete [] m_data;
    m_data = nullptr;
    m_sizeInBytes = 0u;
}

void Buffer::resize(uint64_t sizeInBytes)
{
    if (m_sizeInBytes == sizeInBytes)
        return;

    auto newData = new uint8_t[sizeInBytes];
    if (m_data)
        std::memcpy(newData, m_data, std::min(sizeInBytes, m_sizeInBytes));

    clear();
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

VertexBuffer::VertexBuffer(uint32_t numComponents, uint32_t numVertices, Type type)
    : Buffer(numComponents * numVertices * TypeInfo::size(type))
    , m_numComponents(numComponents)
    , m_numVertices(numVertices)
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
    return m_numVertices;
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

IndexBuffer::IndexBuffer(PrimitiveType primitiveType, uint32_t numIndices, Type type)
    : Buffer(numIndices * TypeInfo::size(type))
    , m_type(type)
    , m_primitiveType(primitiveType)
    , m_numIndices(numIndices)
{
    assert(TypeInfo::isUnsignedInt(m_type));
}

IndexBuffer::~IndexBuffer()
{
}

Type IndexBuffer::type() const
{
    return m_type;
}

PrimitiveType IndexBuffer::primitiveType() const
{
    return m_primitiveType;
}

uint32_t IndexBuffer::numIndices() const
{
    return m_numIndices;
}

Mesh::Mesh()
{
}

void Mesh::declareVertexAttribute(VertexAttribute vertexAttribute, std::shared_ptr<VertexBuffer> vertexBuffer)
{
    m_vertexBuffers.insert({vertexAttribute, vertexBuffer});
}

void Mesh::undeclareVertexAttribute(VertexAttribute vertexAttribute)
{
    m_vertexBuffers.erase(vertexAttribute);
}

void Mesh::addIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer)
{
    m_indexBuffers.insert(indexBuffer);
}

void Mesh::removeIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer)
{
    m_indexBuffers.erase(indexBuffer);
}

const std::unordered_map<VertexAttribute, std::shared_ptr<VertexBuffer> > &Mesh::vertexBuffers() const
{
    return m_vertexBuffers;
}

const std::unordered_set<std::shared_ptr<IndexBuffer> > &Mesh::indexBuffers() const
{
    return m_indexBuffers;
}

}
}

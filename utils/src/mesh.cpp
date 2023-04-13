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
    delete [] m_data;
}

uint64_t Buffer::sizeInBytes() const
{
    return m_sizeInBytes;
}

void Buffer::resize(uint64_t sizeInBytes)
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

IndexBuffer::IndexBuffer(uint32_t numIndices, Type type)
    : Buffer(numIndices * TypeInfo::size(type))
    , m_type(type)
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

uint32_t IndexBuffer::numIndices() const
{
    return static_cast<uint32_t>(m_sizeInBytes / TypeInfo::size(m_type));
}

void IndexBuffer::setNumIndices(uint32_t numIndices)
{
    resize(numIndices * TypeInfo::size(m_type));
}

const void *IndexBuffer::index(uint32_t idx) const
{
    return static_cast<const void*>(m_data + TypeInfo::size(m_type) * idx);
}

void IndexBuffer::setIndex(uint32_t idx, const void *data)
{
    std::memcpy(m_data + TypeInfo::size(m_type) * idx, data, TypeInfo::size(m_type));
}

Mesh::Mesh(PrimitiveType primitiveType)
    : m_primitiveType(primitiveType)
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

void Mesh::attachIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer)
{
    m_indexBuffer = indexBuffer;
}

void Mesh::detachIndexBuffer()
{
    m_indexBuffer = nullptr;
}

PrimitiveType Mesh::primitiveType() const
{
    return m_primitiveType;
}

const std::unordered_map<VertexAttribute, std::shared_ptr<VertexBuffer> > &Mesh::vertexBuffers() const
{
    return m_vertexBuffers;
}

std::shared_ptr<IndexBuffer> Mesh::indexBuffer() const
{
    return m_indexBuffer;
}

std::shared_ptr<Mesh> Mesh::createEmptyMesh(PrimitiveType primitiveType, std::unordered_map<VertexAttribute, std::pair<uint32_t, Type>> attribs, std::optional<Type> indicesType)
{
    auto mesh = std::make_shared<Mesh>(primitiveType);

    for (const auto &[attrib, decl] : attribs)
        mesh->declareVertexAttribute(attrib, std::make_shared<VertexBuffer>(0u, decl.first, decl.second));

    if (indicesType.has_value())
        mesh->attachIndexBuffer(std::make_shared<IndexBuffer>(0u, indicesType.value()));

    return mesh;
}

}
}

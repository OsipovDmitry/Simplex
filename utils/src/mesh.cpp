#include <cstring>
#include <algorithm>
#include <array>

#include <utils/logger.h>
#include <utils/mesh.h>

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

VertexBuffer::VertexBuffer(uint32_t numVertices, uint32_t numComponents, VertexComponentType type)
    : Buffer(0u)
    , m_numComponents(numComponents)
    , m_type(type)
{
    setNumVertices(numVertices);
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
    const uint32_t vertexSize = m_numComponents * sizeOfVertexComponentType(m_type);
    return static_cast<uint32_t>(m_sizeInBytes / vertexSize);
}

void VertexBuffer::setNumVertices(uint32_t numVertices)
{
    resize(numVertices * m_numComponents * sizeOfVertexComponentType(m_type));
}

VertexComponentType VertexBuffer::componentType() const
{
    return m_type;
}

const uint8_t *VertexBuffer::vertex(uint32_t index) const
{
    return m_data + m_numComponents * sizeOfVertexComponentType(m_type) * index;
}

void VertexBuffer::setVertex(uint32_t index, const uint8_t*data)
{
    std::memcpy(m_data + m_numComponents * sizeOfVertexComponentType(m_type) * index,
                data,
                m_numComponents * sizeOfVertexComponentType(m_type));
}

DrawElementsBuffer::DrawElementsBuffer(PrimitiveType primitiveType, uint32_t count, DrawElementsIndexType type, uint32_t baseVertex)
    : DrawElements(primitiveType, count, type, 0, baseVertex)
    , Buffer(0u)
{
    if (m_type == DrawElementsIndexType::Undefined)
        LOG_CRITICAL << "Undefined draw elements index type";

    setNumIndices(m_count);
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
    resize(m_count * indexSize());
}

const void *DrawElementsBuffer::index(uint32_t idx) const
{
    return static_cast<const void*>(m_data + m_offset + indexSize() * idx);
}

void DrawElementsBuffer::setIndex(uint32_t idx, const void *data)
{
    std::memcpy(m_data + m_offset + indexSize() * idx, data, indexSize());
}

Mesh::Mesh()
{
}

void Mesh::attachVertexBuffer(VertexAttribute vertexAttribute, std::shared_ptr<VertexBuffer> vertexBuffer)
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

void Mesh::attachPrimitiveSet(std::shared_ptr<DrawArrays> primitiveSet)
{
    if (!primitiveSet)
        LOG_CRITICAL << "Primitive set can't be nullptr";

    m_primitiveSets.insert(primitiveSet);
}

void Mesh::attachPrimitiveSet(std::shared_ptr<DrawElementsBuffer> primitiveSet)
{
    if (!primitiveSet)
        LOG_CRITICAL << "Primitive set can't be nullptr";

    m_primitiveSets.insert(primitiveSet);
}

void Mesh::detachPrimitiveSet(std::shared_ptr<PrimitiveSet> primitiveSet)
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

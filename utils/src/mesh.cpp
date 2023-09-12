#include <cstring>
#include <cassert>
#include <algorithm>
#include <array>

#include <utils/mesh.h>

#include "tangentspace.h"

namespace simplex
{
namespace utils
{

template<typename V>
inline void calculateTangentSpaceImpl(const std::shared_ptr<DrawArrays> &drawArrays,
                               const VertexBuffer &vertices,
                               const VertexBuffer &texCoords,
                               VertexBuffer &tangents,
                               VertexBuffer &binormals)
{
    calculateTangentSpace(drawArrays,
                          vertices.numComponents(),
                          reinterpret_cast<const V*>(vertices.data()),
                          texCoords.numComponents(),
                          reinterpret_cast<const V*>(texCoords.data()),
                          tangents.numComponents(),
                          reinterpret_cast<V*>(tangents.data()),
                          binormals.numComponents(),
                          reinterpret_cast<V*>(binormals.data()));
}

template<typename V, typename I>
inline void calculateTangentSpaceImpl(const std::shared_ptr<DrawElements> &drawElements,
                               const VertexBuffer &vertices,
                               const VertexBuffer &texCoords,
                               const Buffer &indices,
                               VertexBuffer &tangents,
                               VertexBuffer &binormals)
{
    calculateTangentSpace(drawElements,
                          vertices.numComponents(),
                          reinterpret_cast<const V*>(vertices.data()),
                          texCoords.numComponents(),
                          reinterpret_cast<const V*>(texCoords.data()),
                          reinterpret_cast<const I*>(indices.data()),
                          tangents.numComponents(),
                          reinterpret_cast<V*>(tangents.data()),
                          binormals.numComponents(),
                          reinterpret_cast<V*>(binormals.data()));
}

template<typename V>
inline void calculateTangentSpaceImpl(const std::shared_ptr<DrawElementsBuffer> &drawElementsBuffer,
                               const VertexBuffer &vertices,
                               const VertexBuffer &texCoords,
                               VertexBuffer &tangents,
                               VertexBuffer &binormals)
{
    switch (drawElementsBuffer->type())
    {
    case DrawElementsIndexType::Uint8:
        calculateTangentSpaceImpl<V, uint8_t>(drawElementsBuffer, vertices, texCoords, *drawElementsBuffer, tangents, binormals);
        break;
    case DrawElementsIndexType::Uint16:
        calculateTangentSpaceImpl<V, uint16_t>(drawElementsBuffer, vertices, texCoords, *drawElementsBuffer, tangents, binormals);
        break;
    case DrawElementsIndexType::Uint32:
        calculateTangentSpaceImpl<V, uint32_t>(drawElementsBuffer, vertices, texCoords, *drawElementsBuffer, tangents, binormals);
        break;
    default:
        assert(false);
        break;
    }
}

template<typename V>
inline void orthogonalizeTangentSpaceImpl(uint32_t numVertices,
                                   const VertexBuffer &tangents,
                                   const VertexBuffer &binormals,
                                   const VertexBuffer &normals,
                                   VertexBuffer &result)
{
    orthogonalizeTangentSpace(numVertices,
                              tangents.numComponents(),
                              reinterpret_cast<const V*>(tangents.data()),
                              binormals.numComponents(),
                              reinterpret_cast<const V*>(binormals.data()),
                              normals.numComponents(),
                              reinterpret_cast<const V*>(normals.data()),
                              result.numComponents(),
                              reinterpret_cast<V*>(result.data()));
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
    assert(m_numComponents <= 4u);
    assert(m_type != VertexComponentType::Undefined);

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
    const uint32_t vertexSize = m_numComponents * componentSize();
    return static_cast<uint32_t>(m_sizeInBytes / vertexSize);
}

void VertexBuffer::setNumVertices(uint32_t numVertices)
{
    resize(numVertices * m_numComponents * componentSize());
}

VertexComponentType VertexBuffer::componentType() const
{
    return m_type;
}

uint32_t VertexBuffer::componentSize() const
{
    return componentSize(m_type);
}

const void *VertexBuffer::vertex(uint32_t index) const
{
    return static_cast<const void*>(m_data + m_numComponents * componentSize() * index);
}

void VertexBuffer::setVertex(uint32_t index, const void *data)
{
    std::memcpy(m_data + m_numComponents * componentSize() * index,
                data,
                m_numComponents * componentSize());
}

uint32_t VertexBuffer::componentSize(VertexComponentType type)
{
    static std::array<uint32_t, numElementsVertexComponentType()> s_table {
        0u,
        sizeof(float),
        sizeof(double),
        sizeof(int32_t),
        sizeof(uint32_t)
    };

    return s_table[utils::castFromVertexComponentType(type)];
}

DrawElementsBuffer::DrawElementsBuffer(PrimitiveType primitiveType, uint32_t count, DrawElementsIndexType type, uint32_t baseVertex)
    : DrawElements(primitiveType, count, type, 0, baseVertex)
    , Buffer(0u)
{
    assert(m_type != DrawElementsIndexType::Undefined);

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
    assert(vertexBuffer);
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

const std::unordered_set<std::shared_ptr<PrimitiveSet>> &Mesh::primitiveSets() const
{
    return m_primitiveSets;
}

void Mesh::calculateTangents()
{
    assert(m_vertexBuffers.count(VertexAttribute::Position));
    assert(m_vertexBuffers.count(VertexAttribute::TexCoords));
    assert(m_vertexBuffers.count(VertexAttribute::Normal));

    const auto& vertices = m_vertexBuffers.at(VertexAttribute::Position);
    const auto& texCoords = m_vertexBuffers.at(VertexAttribute::TexCoords);
    const auto& normals = m_vertexBuffers.at(VertexAttribute::Normal);

    const auto numVertices = vertices->numVertices();
    assert(numVertices == texCoords->numVertices());
    assert(numVertices == normals->numVertices());

    const auto vertexComponentType = vertices->componentType();
    assert(vertexComponentType == texCoords->componentType());
    assert(vertexComponentType == normals->componentType());

    auto tangents = std::make_shared<VertexBuffer>(numVertices, 4u, vertexComponentType);
    attachVertexBuffer(VertexAttribute::Tangent, tangents);

    auto t = std::make_shared<VertexBuffer>(numVertices, 3u, vertexComponentType);
    std::memset(t->data(), 0, t->sizeInBytes());

    auto b = std::make_shared<VertexBuffer>(numVertices, 3u, vertexComponentType);
    std::memset(b->data(), 0, t->sizeInBytes());

    for (const auto &primitiveSet : m_primitiveSets)
    {
        if (auto drawArrays = primitiveSet->asDrawArrays(); drawArrays)
        {
            switch (vertexComponentType)
            {
            case VertexComponentType::Single:
                calculateTangentSpaceImpl<float>(drawArrays, *vertices, *texCoords, *t, *b);
                break;
            case VertexComponentType::Double:
                calculateTangentSpaceImpl<double>(drawArrays, *vertices, *texCoords, *t, *b);
                break;
            default:
                assert(false);
                break;
            }


        }
        else if (auto drawElements = primitiveSet->asDrawElements(); drawElements)
        {
            if (auto drawElementsBuffer = drawElements->asDrawElementsBuffer(); drawElementsBuffer)
            {
                switch (vertexComponentType)
                {
                case VertexComponentType::Single:
                    calculateTangentSpaceImpl<float>(drawElementsBuffer, *vertices, *texCoords, *t, *b);
                    break;
                case VertexComponentType::Double:
                    calculateTangentSpaceImpl<double>(drawElementsBuffer, *vertices, *texCoords, *t, *b);
                    break;
                default:
                    assert(false);
                    break;
                }
            }
        }
    }

    switch (vertexComponentType)
    {
    case VertexComponentType::Single:
        orthogonalizeTangentSpaceImpl<float>(numVertices, *t, *b, *normals, *tangents);
        break;
    case VertexComponentType::Double:
        orthogonalizeTangentSpaceImpl<double>(numVertices, *t, *b, *normals, *tangents);
        break;
    default:
        assert(false);
        break;
    }
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

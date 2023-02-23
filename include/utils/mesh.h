#ifndef MESH_H
#define MESH_H

#include <cstdint>
#include <unordered_set>
#include <unordered_map>
#include <memory>

#include <utils/utilsglobal.h>
#include <utils/noncopyble.h>
#include <utils/types.h>

namespace simplex
{
namespace utils
{

class UTILS_SHARED_EXPORT Buffer
{
    NONCOPYBLE(Buffer)
public:
    explicit Buffer(uint64_t);
    virtual ~Buffer();

    uint64_t sizeInBytes() const;

    void clear();
    void resize(uint64_t);

    uint8_t *data();
    const uint8_t *data() const;

protected:
    uint8_t *m_data;
    uint64_t m_sizeInBytes;
};

class UTILS_SHARED_EXPORT VertexBuffer : public Buffer
{
public:
    VertexBuffer(uint32_t, uint32_t, Type = Type::Single);
    ~VertexBuffer() override;

    uint32_t numComponents() const;
    uint32_t numVertices() const;
    Type type() const;

    const void* vertex(uint32_t) const;
    void setVertex(uint32_t, const void*);

protected:
    uint32_t m_numComponents;
    uint32_t m_numVertices;
    Type m_type;
};

class UTILS_SHARED_EXPORT IndexBuffer : public Buffer
{
public:
    IndexBuffer(PrimitiveType, uint32_t, Type);
    ~IndexBuffer() override;

    Type type() const;
    PrimitiveType primitiveType() const;
    uint32_t numIndices() const;

protected:
    Type m_type;
    PrimitiveType m_primitiveType;
    uint32_t m_numIndices;
};

class UTILS_SHARED_EXPORT Mesh
{
    NONCOPYBLE(Mesh)
public:
    Mesh();

    void declareVertexAttribute(VertexAttribute, std::shared_ptr<VertexBuffer>);
    void undeclareVertexAttribute(VertexAttribute);

    void addIndexBuffer(std::shared_ptr<IndexBuffer>);
    void removeIndexBuffer(std::shared_ptr<IndexBuffer>);

    const std::unordered_map<VertexAttribute, std::shared_ptr<VertexBuffer>>& vertexBuffers() const;
    const std::unordered_set<std::shared_ptr<IndexBuffer>>& indexBuffers() const;

private:
    std::unordered_map<VertexAttribute, std::shared_ptr<VertexBuffer>> m_vertexBuffers;
    std::unordered_set<std::shared_ptr<IndexBuffer>> m_indexBuffers;
};

}
}

#endif // MESH_H

#ifndef UTILS_MESH_H
#define UTILS_MESH_H

#include <cstdint>
#include <unordered_map>
#include <memory>
#include <optional>

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
    VertexBuffer(uint32_t numVertices, uint32_t numComponents, Type = Type::Single);
    ~VertexBuffer() override;

    uint32_t numComponents() const;
    Type type() const;

    uint32_t numVertices() const;
    void setNumVertices(uint32_t);

    const void *vertex(uint32_t) const;
    void setVertex(uint32_t, const void*);

protected:
    uint32_t m_numComponents;
    Type m_type;
};

class UTILS_SHARED_EXPORT IndexBuffer : public Buffer
{
public:
    IndexBuffer(uint32_t, Type);
    ~IndexBuffer() override;

    Type type() const;

    uint32_t numIndices() const;
    void setNumIndices(uint32_t);

    const void *index(uint32_t) const;
    void setIndex(uint32_t, const void*);

protected:
    Type m_type;
};

class UTILS_SHARED_EXPORT Mesh
{
    NONCOPYBLE(Mesh)
public:
    Mesh(PrimitiveType = PrimitiveType::Triangles);

    void declareVertexAttribute(VertexAttribute, std::shared_ptr<VertexBuffer>);
    void undeclareVertexAttribute(VertexAttribute);

    void attachIndexBuffer(std::shared_ptr<IndexBuffer>);
    void detachIndexBuffer();

    PrimitiveType primitiveType() const;

    const std::unordered_map<VertexAttribute, std::shared_ptr<VertexBuffer>>& vertexBuffers() const;
    std::shared_ptr<IndexBuffer> indexBuffer() const;

    static std::shared_ptr<Mesh> createEmptyMesh(PrimitiveType, std::unordered_map<VertexAttribute, std::pair<uint32_t, Type>>, std::optional<Type> = Type::Uint32);

private:
    std::unordered_map<VertexAttribute, std::shared_ptr<VertexBuffer>> m_vertexBuffers;
    std::shared_ptr<IndexBuffer> m_indexBuffer;
    PrimitiveType m_primitiveType;
};

}
}

#endif // UTILS_MESH_H

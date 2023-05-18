#ifndef UTILS_MESH_H
#define UTILS_MESH_H

#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <memory>

#include <utils/utilsglobal.h>
#include <utils/noncopyble.h>
#include <utils/types.h>
#include <utils/primitiveset.h>

namespace simplex
{
namespace utils
{

class UTILS_SHARED_EXPORT Buffer
{
    NONCOPYBLE(Buffer)
public:
    explicit Buffer(size_t);
    virtual ~Buffer();

    size_t sizeInBytes() const;
    void resize(size_t);

    uint8_t *data();
    const uint8_t *data() const;

protected:
    uint8_t *m_data;
    size_t m_sizeInBytes;
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

class UTILS_SHARED_EXPORT DrawElementsBuffer : public DrawElements, public Buffer
{
public:
    DrawElementsBuffer(PrimitiveType, uint32_t count, Type type, uint32_t baseVertex);
    ~DrawElementsBuffer() override;

    std::shared_ptr<DrawElementsBuffer> asDrawElementsBuffer() override;
    std::shared_ptr<const DrawElementsBuffer> asDrawElementsBuffer() const override;

    uint32_t numIndices() const;
    void setNumIndices(uint32_t);

    const void *index(uint32_t) const;
    void setIndex(uint32_t, const void*);
};

class UTILS_SHARED_EXPORT Mesh
{
    NONCOPYBLE(Mesh)
public:
    Mesh();

    void attachVertexBuffer(VertexAttribute, std::shared_ptr<VertexBuffer>);
    void detachVertexBuffer(VertexAttribute);
    const std::unordered_map<VertexAttribute, std::shared_ptr<VertexBuffer>> &vertexBuffers() const;

    void attachPrimitiveSet(std::shared_ptr<DrawArrays>);
    void attachPrimitiveSet(std::shared_ptr<DrawElementsBuffer>);
    void detachPrimitiveSet(std::shared_ptr<PrimitiveSet>);
    const std::unordered_set<std::shared_ptr<PrimitiveSet>> &primitiveSets() const;

private:
    std::unordered_map<VertexAttribute, std::shared_ptr<VertexBuffer>> m_vertexBuffers;
    std::unordered_set<std::shared_ptr<PrimitiveSet>> m_primitiveSets;
};

}
}

#endif // UTILS_MESH_H

#ifndef UTILS_MESH_H
#define UTILS_MESH_H

#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <memory>

#include <utils/utilsglobal.h>
#include <utils/noncopyble.h>
#include <utils/primitiveset.h>

namespace simplex
{
namespace utils
{

ENUMCLASS(VertexComponentType, uint16_t,
          Single,
          Double,
          Int8,
          Uint8,
          Int16,
          Uint16,
          Int32,
          Uint32)
size_t UTILS_SHARED_EXPORT sizeOfVertexComponentType(VertexComponentType);

template <typename T>
inline VertexComponentType toVertexComponentType() { return VertexComponentType::Count; }
template<> inline VertexComponentType toVertexComponentType<float>() { return VertexComponentType::Single; }
template<> inline VertexComponentType toVertexComponentType<double>() { return VertexComponentType::Double; }
template<> inline VertexComponentType toVertexComponentType<int8_t>() { return VertexComponentType::Int8; }
template<> inline VertexComponentType toVertexComponentType<uint8_t>() { return VertexComponentType::Uint8; }
template<> inline VertexComponentType toVertexComponentType<int16_t>() { return VertexComponentType::Int16; }
template<> inline VertexComponentType toVertexComponentType<uint16_t>() { return VertexComponentType::Uint16; }
template<> inline VertexComponentType toVertexComponentType<int32_t>() { return VertexComponentType::Int32; }
template<> inline VertexComponentType toVertexComponentType<uint32_t>() { return VertexComponentType::Uint32; }

ENUMCLASS(VertexAttribute, uint16_t,
          Position,
          Normal,
          TexCoords,
          BonesIDs,
          BonesWeights,
          Tangent,
          Color)

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
    VertexBuffer(size_t numVertices, uint32_t numComponents, VertexComponentType);
    ~VertexBuffer() override;

    uint32_t numComponents() const;
    VertexComponentType componentType() const;

    size_t numVertices() const;
    void setNumVertices(size_t);

    const uint8_t *vertex(size_t) const;
    void setVertex(size_t, const uint8_t*);

    std::shared_ptr<VertexBuffer> copy() const;
    void convert(uint32_t, VertexComponentType);
    std::shared_ptr<VertexBuffer> converted(uint32_t, VertexComponentType) const;

protected:
    uint32_t m_numComponents;
    VertexComponentType m_type;
};

class UTILS_SHARED_EXPORT DrawElementsBuffer : public DrawElements, public Buffer
{
public:
    DrawElementsBuffer(PrimitiveType, uint32_t count, DrawElementsIndexType, size_t baseVertex);
    ~DrawElementsBuffer() override;

    std::shared_ptr<DrawElementsBuffer> asDrawElementsBuffer() override;
    std::shared_ptr<const DrawElementsBuffer> asDrawElementsBuffer() const override;

    size_t numIndices() const;
    void setNumIndices(size_t);

    const void *index(size_t) const;
    void setIndex(size_t, const void*);

    std::shared_ptr<DrawElementsBuffer> copy() const;

    void convertToIndexType(DrawElementsIndexType);
    std::shared_ptr<DrawElementsBuffer> convertedToIndexType(DrawElementsIndexType) const;

    void convertToTriangles();
    std::shared_ptr<DrawElementsBuffer> convertedToTriangles() const;

    void applyBaseVertex();
    std::shared_ptr<DrawElementsBuffer> appliedBaseVertex() const;
};

class UTILS_SHARED_EXPORT Mesh
{
    NONCOPYBLE(Mesh)
public:
    Mesh();

    void attachVertexBuffer(VertexAttribute, const std::shared_ptr<VertexBuffer>&);
    void detachVertexBuffer(VertexAttribute);
    const std::unordered_map<VertexAttribute, std::shared_ptr<VertexBuffer>> &vertexBuffers() const;

    void attachPrimitiveSet(const std::shared_ptr<DrawArrays>&);
    void attachPrimitiveSet(const std::shared_ptr<DrawElementsBuffer>&);
    void detachPrimitiveSet(const std::shared_ptr<PrimitiveSet>&);
    const std::unordered_set<std::shared_ptr<PrimitiveSet>> &primitiveSets() const;

    static std::shared_ptr<Mesh> createEmptyMesh(const std::unordered_map<VertexAttribute, std::tuple<uint32_t, VertexComponentType>>&);

private:
    std::unordered_map<VertexAttribute, std::shared_ptr<VertexBuffer>> m_vertexBuffers;
    std::unordered_set<std::shared_ptr<PrimitiveSet>> m_primitiveSets;
};

}
}

#endif // UTILS_MESH_H

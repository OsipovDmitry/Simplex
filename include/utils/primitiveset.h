#ifndef UTILS_PRIMITIVESET_H
#define UTILS_PRIMITIVESET_H

#include <memory>

#include <utils/utilsglobal.h>
#include <utils/noncopyble.h>
#include <utils/enumclass.h>
#include <utils/forwarddecl.h>

namespace simplex
{
namespace utils
{

ENUMCLASS(PrimitiveType, uint16_t,
          Points,
          Lines,
          LineStrip,
          Triangles,
          TriangleStrip,
          TriangleFan)

ENUMCLASS(DrawElementsIndexType, uint16_t,
          Uint8,
          Uint16,
          Uint32)

class UTILS_SHARED_EXPORT PrimitiveSet : public std::enable_shared_from_this<PrimitiveSet>
{
    NONCOPYBLE(PrimitiveSet)
public:
    PrimitiveSet(PrimitiveType);
    virtual ~PrimitiveSet() = default;

    virtual std::shared_ptr<PrimitiveSet> asPrimitiveSet();
    virtual std::shared_ptr<const PrimitiveSet> asPrimitiveSet() const;

    virtual std::shared_ptr<DrawArrays> asDrawArrays();
    virtual std::shared_ptr<const DrawArrays> asDrawArrays() const;

    virtual std::shared_ptr<DrawElements> asDrawElements();
    virtual std::shared_ptr<const DrawElements> asDrawElements() const;

    PrimitiveType primitiveType() const;

protected:
    PrimitiveType m_primitiveType;
};

class UTILS_SHARED_EXPORT DrawArrays : public PrimitiveSet
{
public:
    DrawArrays(PrimitiveType, size_t first, size_t count);

    std::shared_ptr<DrawArrays> asDrawArrays() override;
    std::shared_ptr<const DrawArrays> asDrawArrays() const override;

    size_t first() const;
    size_t count() const;

    std::shared_ptr<DrawElementsBuffer> convertToElements(DrawElementsIndexType) const;

protected:
    size_t m_first;
    size_t m_count;
};

class UTILS_SHARED_EXPORT DrawElements : public PrimitiveSet
{
public:
    DrawElements(PrimitiveType, size_t count, DrawElementsIndexType indexType, size_t offset, size_t baseVertex);

    std::shared_ptr<DrawElements> asDrawElements() override;
    std::shared_ptr<const DrawElements> asDrawElements() const override;

    virtual std::shared_ptr<DrawElementsBuffer> asDrawElementsBuffer();
    virtual std::shared_ptr<const DrawElementsBuffer> asDrawElementsBuffer() const;

    size_t count() const;
    DrawElementsIndexType indexType() const;
    size_t offset() const;
    size_t baseVertex() const;

    uint32_t indexSize() const;
    static uint32_t indexSize(DrawElementsIndexType);

protected:
    size_t m_offset;
    size_t m_count;
    size_t m_baseVertex;
    DrawElementsIndexType m_indexType;
};

template<typename T> inline constexpr DrawElementsIndexType toDrawElementsIndexType() { return DrawElementsIndexType::Count; }
template<> inline constexpr DrawElementsIndexType toDrawElementsIndexType<uint8_t>() { return DrawElementsIndexType::Uint8; }
template<> inline constexpr DrawElementsIndexType toDrawElementsIndexType<uint16_t>() { return DrawElementsIndexType::Uint16; }
template<> inline constexpr DrawElementsIndexType toDrawElementsIndexType<uint32_t>() { return DrawElementsIndexType::Uint32; }

}
}

#endif // UTILS_PRIMITIVESET_H

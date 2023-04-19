#ifndef UTILS_PRIMITIVESET_H
#define UTILS_PRIMITIVESET_H

#include <memory>

#include <utils/utilsglobal.h>
#include <utils/forwarddecl.h>
#include <utils/noncopyble.h>

namespace simplex
{
namespace utils
{

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
    DrawArrays(PrimitiveType, uint32_t first, uint32_t count);

    std::shared_ptr<DrawArrays> asDrawArrays() override;
    std::shared_ptr<const DrawArrays> asDrawArrays() const override;

    uint32_t first() const;
    uint32_t count() const;

protected:
    uint32_t m_first;
    uint32_t m_count;
};

class UTILS_SHARED_EXPORT DrawElements : public PrimitiveSet
{
public:
    DrawElements(PrimitiveType, uint32_t count, Type type, size_t offset, uint32_t baseVertex);

    std::shared_ptr<DrawElements> asDrawElements() override;
    std::shared_ptr<const DrawElements> asDrawElements() const override;

    virtual std::shared_ptr<DrawElementsBuffer> asDrawElementsBuffer();
    virtual std::shared_ptr<const DrawElementsBuffer> asDrawElementsBuffer() const;

    uint32_t count() const;
    Type type() const;
    size_t offset() const;
    uint32_t baseVertex() const;

protected:
    uint32_t m_count;
    Type m_type;
    size_t m_offset;
    uint32_t m_baseVertex;
};

}
}

#endif // UTILS_PRIMITIVESET_H

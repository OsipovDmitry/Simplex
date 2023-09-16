#include <utils/logger.h>
#include <utils/primitiveset.h>

namespace simplex
{
namespace utils
{

PrimitiveSet::PrimitiveSet(PrimitiveType primitiveType)
    : m_primitiveType(primitiveType)
{
    if (m_primitiveType == PrimitiveType::Undefined)
        LOG_CRITICAL << "Undefined primitive type";
}

std::shared_ptr<PrimitiveSet> PrimitiveSet::asPrimitiveSet()
{
    return shared_from_this();
}

std::shared_ptr<const PrimitiveSet> PrimitiveSet::asPrimitiveSet() const
{
    return shared_from_this();
}

std::shared_ptr<DrawArrays> PrimitiveSet::asDrawArrays()
{
    return nullptr;
}

std::shared_ptr<const DrawArrays> PrimitiveSet::asDrawArrays() const
{
    return nullptr;
}

std::shared_ptr<DrawElements> PrimitiveSet::asDrawElements()
{
    return nullptr;
}

std::shared_ptr<const DrawElements> PrimitiveSet::asDrawElements() const
{
    return nullptr;
}

PrimitiveType PrimitiveSet::primitiveType() const
{
    return m_primitiveType;
}

DrawArrays::DrawArrays(PrimitiveType pt, uint32_t first, uint32_t count)
    : PrimitiveSet(pt)
    , m_first(first)
    , m_count(count)
{
}

std::shared_ptr<DrawArrays> DrawArrays::asDrawArrays()
{
    return std::dynamic_pointer_cast<DrawArrays>(shared_from_this());
}

std::shared_ptr<const DrawArrays> DrawArrays::asDrawArrays() const
{
    return std::dynamic_pointer_cast<const DrawArrays>(shared_from_this());
}

uint32_t DrawArrays::first() const
{
    return m_first;
}

uint32_t DrawArrays::count() const
{
    return m_count;
}

DrawElements::DrawElements(PrimitiveType pt, uint32_t count, DrawElementsIndexType type, uint64_t offset, uint32_t baseVertex)
    : PrimitiveSet(pt)
    , m_offset(offset)
    , m_count(count)
    , m_baseVertex(baseVertex)
    , m_type(type)
{
    if (m_type == DrawElementsIndexType::Undefined)
        LOG_CRITICAL << "Undefined draw elements index type";
}

std::shared_ptr<DrawElements> DrawElements::asDrawElements()
{
    return std::dynamic_pointer_cast<DrawElements>(shared_from_this());
}

std::shared_ptr<const DrawElements> DrawElements::asDrawElements() const
{
    return std::dynamic_pointer_cast<const DrawElements>(shared_from_this());
}

std::shared_ptr<DrawElementsBuffer> DrawElements::asDrawElementsBuffer()
{
    return nullptr;
}

std::shared_ptr<const DrawElementsBuffer> DrawElements::asDrawElementsBuffer() const
{
    return nullptr;
}

uint32_t DrawElements::count() const
{
    return m_count;
}

DrawElementsIndexType DrawElements::type() const
{
    return m_type;
}

size_t DrawElements::offset() const
{
    return m_offset;
}

uint32_t DrawElements::baseVertex() const
{
    return m_baseVertex;
}

uint32_t DrawElements::indexSize() const
{
    return indexSize(m_type);
}

uint32_t DrawElements::indexSize(DrawElementsIndexType type)
{
    static std::array<uint32_t, numElementsDrawElementsIndexType()> s_table {
        0u,
        sizeof(uint8_t),
        sizeof(uint16_t),
        sizeof(uint32_t)
    };

    return s_table[utils::castFromDrawElementsIndexType(type)];
}

}
}

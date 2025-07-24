#include <utils/logger.h>
#include <utils/primitiveset.h>
#include <utils/mesh.h>

namespace simplex
{
namespace utils
{

PrimitiveSet::PrimitiveSet(PrimitiveType primitiveType)
    : m_primitiveType(primitiveType)
{
    if (m_primitiveType == PrimitiveType::Count)
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

DrawArrays::DrawArrays(PrimitiveType pt, size_t first, size_t count)
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

size_t DrawArrays::first() const
{
    return m_first;
}

size_t DrawArrays::count() const
{
    return m_count;
}

template <typename T>
static void convertToIndices(size_t first, size_t count, T *data)
{
    for (size_t i = 0u; i < count; ++i)
        data[i] = static_cast<T>(first + i);
}

std::shared_ptr<DrawElementsBuffer> DrawArrays::convertToElements(DrawElementsIndexType indexType) const
{
    auto result = std::make_shared<DrawElementsBuffer>(primitiveType(), count(), indexType, 0u);

    switch (indexType)
    {
    case DrawElementsIndexType::Uint8:
    {
        convertToIndices(first(), count(), reinterpret_cast<uint8_t*>(result->data()));
        break;
    }
    case DrawElementsIndexType::Uint16:
    {
        convertToIndices(first(), count(), reinterpret_cast<uint16_t*>(result->data()));
        break;
    }
    case DrawElementsIndexType::Uint32:
    {
        convertToIndices(first(), count(), reinterpret_cast<uint32_t*>(result->data()));
        break;
    }
    default:
    {
        result = nullptr;
        LOG_CRITICAL << "Undefined draw elements index type";
        break;
    }
    }

    return result;
}

DrawElements::DrawElements(PrimitiveType pt, size_t count, DrawElementsIndexType indexType, size_t offset, size_t baseVertex)
    : PrimitiveSet(pt)
    , m_offset(offset)
    , m_count(count)
    , m_baseVertex(baseVertex)
    , m_indexType(indexType)
{
    if (m_indexType == DrawElementsIndexType::Count)
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

size_t DrawElements::count() const
{
    return m_count;
}

DrawElementsIndexType DrawElements::indexType() const
{
    return m_indexType;
}

size_t DrawElements::offset() const
{
    return m_offset;
}

size_t DrawElements::baseVertex() const
{
    return m_baseVertex;
}

uint32_t DrawElements::indexSize() const
{
    return indexSize(m_indexType);
}

uint32_t DrawElements::indexSize(DrawElementsIndexType type)
{
    static std::array<uint32_t, numElementsDrawElementsIndexType()> s_table {
        sizeof(uint8_t),
        sizeof(uint16_t),
        sizeof(uint32_t)
    };

    return s_table[utils::castFromDrawElementsIndexType(type)];
}

}
}

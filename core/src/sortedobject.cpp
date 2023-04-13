#include <core/sortedobject.h>

namespace simplex
{
namespace core
{

SortedObject::SortedObject(uint32_t index)
    : m_sortIndex(index)
{
}

uint32_t SortedObject::sortIndex() const
{
    return m_sortIndex;
}

void SortedObject::setSortIndex(uint32_t index)
{
    m_sortIndex = index;
}

void SortedObject::setUnsortedIndex()
{
    setSortIndex(static_cast<uint32_t>(-1));
}

bool SortedObject::Comparator::operator ()(const std::shared_ptr<SortedObject> &o1, const std::shared_ptr<SortedObject> &o2)
{
    auto i1 = o1->sortIndex();
    auto i2 = o2->sortIndex();

    if (i1 == static_cast<uint32_t>(-1))
        return false;

    if (i2 == static_cast<uint32_t>(-1))
        return true;

    return i1 < i2;
}

}
}

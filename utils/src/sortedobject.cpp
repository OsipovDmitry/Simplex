#include <utils/sortedobject.h>

namespace simplex
{
namespace utils
{

const uint32_t SortedObject::s_unsortedIndex = static_cast<uint32_t>(-1);

SortedObject::SortedObject(uint32_t index)
    : m_sortIndex(index)
{
}

SortedObject::~SortedObject() = default;

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
    setSortIndex(s_unsortedIndex);
}

bool SortedObjectComparator::operator ()(const std::shared_ptr<SortedObject> &o1, const std::shared_ptr<SortedObject> &o2)
{
    auto i1 = o1->sortIndex();
    auto i2 = o2->sortIndex();

    if (i1 == SortedObject::s_unsortedIndex)
        return false;

    if (i2 == SortedObject::s_unsortedIndex)
        return true;

    return i1 < i2;
}

}
}

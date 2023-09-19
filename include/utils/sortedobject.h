#ifndef UTILS_SORTEDOBJECT_H
#define UTILS_SORTEDOBJECT_H

#include <inttypes.h>
#include <memory>

#include <utils/utilsglobal.h>

namespace simplex
{
namespace utils
{

class UTILS_SHARED_EXPORT SortedObject
{
public:
    explicit SortedObject(uint32_t = s_unsortedIndex);
    virtual ~SortedObject();

    uint32_t sortIndex() const;
    void setSortIndex(uint32_t);
    void setUnsortedIndex();

    static const uint32_t s_unsortedIndex;

protected:
    uint32_t m_sortIndex;
};

struct UTILS_SHARED_EXPORT SortedObjectComparator
{
    bool operator ()(const std::shared_ptr<SortedObject>&, const std::shared_ptr<SortedObject>&);
};

}
}

#endif // UTILS_SORTEDOBJECT_H

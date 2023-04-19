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
    SortedObject(uint32_t = static_cast<uint32_t>(-1));
    virtual ~SortedObject() = default;

    uint32_t sortIndex() const;
    void setSortIndex(uint32_t);
    void setUnsortedIndex();

    struct UTILS_SHARED_EXPORT Comparator
    {
        bool operator ()(const std::shared_ptr<SortedObject>&, const std::shared_ptr<SortedObject>&);
    };

protected:
    uint32_t m_sortIndex;
};

}
}

#endif // UTILS_SORTEDOBJECT_H

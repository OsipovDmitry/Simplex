#ifndef CORE_SORTEDOBJECT_H
#define CORE_SORTEDOBJECT_H

#include <inttypes.h>
#include <memory>

#include <core/coreglobal.h>

namespace simplex
{
namespace core
{

class CORE_SHARED_EXPORT SortedObject
{
public:
    SortedObject(uint32_t = static_cast<uint32_t>(-1));
    virtual ~SortedObject() = default;

    uint32_t sortIndex() const;
    void setSortIndex(uint32_t);
    void setUnsortedIndex();

    struct Comparator
    {
        bool operator ()(const std::shared_ptr<SortedObject>&, const std::shared_ptr<SortedObject>&);
    };

protected:
    uint32_t m_sortIndex;
};

}
}

#endif // CORE_SORTEDOBJECT_H

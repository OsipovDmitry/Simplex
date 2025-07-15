#ifndef UTILS_IDGENERATOR_H
#define UTILS_IDGENERATOR_H

#include <vector>
#include <limits>

#include <utils/logger.h>

namespace simplex
{
namespace utils
{

template <typename T>
struct IDGeneratorT
{
    static_assert(std::numeric_limits<T>::is_integer && std::numeric_limits<T>::is_unsigned, "The base type of IDGenerator must be unsigned integer");
public:
    using value_type = T;

    IDGeneratorT()
        : m_nextID(static_cast<value_type>(0))
    {}

    value_type generate()
    {
        value_type result = static_cast<value_type>(0);
        if (m_freeIDs.empty())
        {
            if (m_nextID == last()) // last is reserved for "undefined" id
                LOG_CRITICAL << "IDs run out";
            result = m_nextID++;
        }
        else
        {
            result = m_freeIDs.back();
            m_freeIDs.pop_back();
        }
        return result;
    }

    void clear(value_type value)
    {
        if (value >= m_nextID)
            LOG_CRITICAL << "ID is not registerd";
        if (value + static_cast<value_type>(1) != m_nextID)
            m_freeIDs.push_back(value);
        else
            --m_nextID;
    }

    static value_type last()
    {
        return std::numeric_limits<value_type>::max();
    }

private:
    value_type m_nextID;
    std::vector<value_type> m_freeIDs;
};

}
}

#endif // UTILS_IDGENERATOR_H
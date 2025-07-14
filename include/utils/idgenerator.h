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
struct IDGenerator
{
    static_assert(std::numeric_limits<T>::is_integer && std::numeric_limits<T>::is_unsigned, "The base type of IDGenerator must be unsigned integer");
public:
    IDGenerator()
        : m_nextID(static_cast<T>(0))
    {}

    T generate()
    {
        T result = static_cast<T>(0);
        if (m_freeIDs.empty())
        {
            if (m_nextID == std::numeric_limits<T>::max())
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

    void clear(T value)
    {
        if (value >= m_nextID)
            LOG_CRITICAL << "ID is not registerd";
        if (value + static_cast<T>(1) != m_nextID)
            m_freeIDs.push_back(value);
        else
            --m_nextID;
    }

private:
    T m_nextID;
    std::vector<T> m_freeIDs;
};

}
}

#endif // UTILS_IDGENERATOR_H
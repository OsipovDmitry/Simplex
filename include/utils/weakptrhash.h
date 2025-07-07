#ifndef UTILS_FILEEXTENSION_H
#define UTILS_FILEEXTENSION_H

#include <memory>

namespace simplex
{
namespace utils
{

template <typename T>
inline bool operator ==(const std::weak_ptr<T>& l, const std::weak_ptr<T>& r)
{
    if (l.expired() || r.expired())
        return false;

    return l.lock() == r.lock();
}

template <typename T>
struct WeakPtrHash
{
public:
    size_t operator ()(const std::weak_ptr<T>& value) const
    {
        return value.expired() ? 0u : std::hash<std::shared_ptr<T>>()(value.lock());
    }
};

}
}

#endif // UTILS_FILEEXTENSION_H

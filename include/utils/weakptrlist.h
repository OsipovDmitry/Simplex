#ifndef UTILS_FILEEXTENSION_H
#define UTILS_FILEEXTENSION_H

#include <memory>
#include <list>

namespace simplex
{
namespace utils
{

template<typename T>
class WeakPtrList : public std::list<std::weak_ptr<T>>
{
public:
    WeakPtrList()
    {}

    typename std::list<std::weak_ptr<T>>::iterator find(const std::shared_ptr<T>& value)
    {
        for (auto it = begin(); it != end(); ++it)
        {
            if (it->expired())
                continue;

            if (auto ptr = it->lock(); ptr == value)
                return it;
        }

        return end();
    }

    template<typename UnaryPredicate>
    typename std::list<std::weak_ptr<T>>::iterator find_if(UnaryPredicate pred)
    {
        for (auto it = begin(); it != end(); ++it)
        {
            if (it->expired())
                continue;

            if (pred(it->lock()))
                return it;
        }

        return end();
    }

    typename std::list<std::weak_ptr<T>>::iterator find_any()
    {
        for (auto it = begin(); it != end(); ++it)
        {
            if (!it->expired())
                return it;
        }

        return end();
    }
};

}
}

#endif // UTILS_FILEEXTENSION_H

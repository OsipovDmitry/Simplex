#include <algorithm>

#include "contextgroup_p.h"

namespace renderer
{

ContextGroupPrivate::ContextGroupPrivate()
{
}

ContextGroupPrivate::~ContextGroupPrivate()
{
}

void ContextGroupPrivate::add(ContextPtr context)
{
    if (find(context))
        return;

    contexts.push_back(context);
}

void ContextGroupPrivate::remove(ContextPtr context)
{
    auto iter = std::find_if(contexts.begin(), contexts.end(), [context](renderer::ContextWPtr c){
        return c.lock() == context;
    });
    if (iter != contexts.end())
        contexts.erase(iter);
}

bool ContextGroupPrivate::find(ContextPtr context) const
{
    auto iter = std::find_if(contexts.cbegin(), contexts.cend(), [context](renderer::ContextWPtr c){
        return c.lock() == context;
    });
    return iter != contexts.cend();
}

}

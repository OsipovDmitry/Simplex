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

bool ContextGroupPrivate::find(ContextPtr context) const
{
    auto iter = std::find_if(contexts.cbegin(), contexts.cend(), [context](renderer::ContextWPtr c){
        return c.lock() == context;
    });
    return iter != contexts.cend();
}

}

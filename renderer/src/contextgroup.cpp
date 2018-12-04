#include <renderer/contextgroup.h>

#include "contextgroup_p.h"

namespace renderer {

ContextGroup::ContextGroup() :
    m_(new ContextGroupPrivate())
{
}

ContextGroup::~ContextGroup()
{
    delete m_;
}

const std::list<ContextWPtr> &ContextGroup::contexts() const
{
    return m_->contexts;
}



}

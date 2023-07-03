#include "viewportprivate.h"

namespace simplex
{
namespace core
{

ViewportPrivate::ViewportPrivate()
    : m_policy(Viewport::SizePolicy::Defined)
    , m_size(0u, 0u, 1u, 1u)
{
}

ViewportPrivate::~ViewportPrivate()
{
}

Viewport::SizePolicy &ViewportPrivate::sizePolicy()
{
    return m_policy;
}

glm::uvec4 &ViewportPrivate::size()
{
    return m_size;
}


}
}

#include <core/viewport.h>

#include "viewportprivate.h"

namespace simplex
{
namespace core
{

Viewport::Viewport()
    : m_(std::make_unique<ViewportPrivate>())
{
}

Viewport::~Viewport()
{
}

Viewport::SizePolicy Viewport::sizePolicy() const
{
    return m_->sizePolicy();
}

void Viewport::setSizePolicy(SizePolicy value)
{
    m_->sizePolicy() = value;
}

const glm::uvec4 &Viewport::size() const
{
    return m_->size();
}

void Viewport::setSize(const glm::uvec4 &value)
{
    m_->size() = value;
}

}
}


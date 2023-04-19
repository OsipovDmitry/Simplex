#include <core/renderinfo.h>

#include "renderinfoprivate.h"

namespace simplex
{
namespace core
{

RenderInfo::RenderInfo()
    : m_(std::make_unique<RenderInfoPrivate>())
{
    setViewport(glm::uvec2(1u, 1u));
    setViewMatrix(glm::mat4(1.f));
    setProjectionMatrix(glm::mat4(1.f));
}

RenderInfo::~RenderInfo()
{
}

const glm::uvec2 &RenderInfo::viewport() const
{
    return m_->viewport();
}

void RenderInfo::setViewport(const glm::uvec2 &vp)
{
    m_->viewport() = vp;
}

const glm::mat4 &RenderInfo::viewMatrix() const
{
    return m_->viewMatrix();
}

void RenderInfo::setViewMatrix(const glm::mat4 &vm)
{
    m_->viewMatrix() = vm;
}

const glm::mat4 &RenderInfo::projectionMatrix() const
{
    return m_->projectionMatrix();
}

void RenderInfo::setProjectionMatrix(const glm::mat4 &pm)
{
    m_->projectionMatrix() = pm;
}


}
}

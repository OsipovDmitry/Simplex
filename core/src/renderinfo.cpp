#include <core/renderinfo.h>

#include "renderinfoprivate.h"

namespace simplex
{
namespace core
{

RenderInfo::RenderInfo(const glm::uvec2 &viewport,
                       const glm::mat4 &viewMatrix,
                       const glm::mat4 &projectionMatrix)
    : m_(std::make_unique<RenderInfoPrivate>())
{
    setViewport(viewport);
    setViewMatrix(viewMatrix);
    setProjectionMatrix(projectionMatrix);
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

const glm::mat4 &RenderInfo::viewMatrixInverse() const
{
    return m_->viewMatrixInverse();
}

const glm::vec3 &RenderInfo::viewPosition() const
{
    return m_->viewPosition();
}

const glm::vec3 &RenderInfo::viewXDirection() const
{
    return m_->viewXDirection();
}

const glm::vec3 &RenderInfo::viewYDirection() const
{
    return m_->viewYDirection();
}

const glm::vec3 &RenderInfo::viewZDirection() const
{
    return m_->viewZDirection();
}

void RenderInfo::setViewMatrix(const glm::mat4 &vm)
{
    m_->viewMatrix() = vm;
    m_->viewMatrixInverse() = glm::inverse(m_->viewMatrix());
    m_->viewProjectionMatrix() = m_->projectionMatrix() * m_->viewMatrix();
    m_->viewProjectionMatrixInverse() = glm::inverse(m_->viewProjectionMatrix());

    m_->viewPosition() = glm::vec3(m_->viewMatrixInverse() * glm::vec4(0.f, 0.f, 0.f, 1.f));
    m_->viewXDirection() = glm::vec3(m_->viewMatrixInverse() * glm::vec4(1.f, 0.f, 0.f, 0.f));
    m_->viewYDirection() = glm::vec3(m_->viewMatrixInverse() * glm::vec4(0.f, 1.f, 0.f, 0.f));
    m_->viewZDirection() = glm::vec3(m_->viewMatrixInverse() * glm::vec4(0.f, 0.f, 1.f, 0.f));
}

const glm::mat4 &RenderInfo::projectionMatrix() const
{
    return m_->projectionMatrix();
}

const glm::mat4 &RenderInfo::projectionMatrixInverse() const
{
    return m_->projectionMatrixInverse();
}

void RenderInfo::setProjectionMatrix(const glm::mat4 &pm)
{
    m_->projectionMatrix() = pm;
    m_->projectionMatrixInverse() = glm::inverse(m_->projectionMatrix());
    m_->viewProjectionMatrix() = m_->projectionMatrix() * m_->viewMatrix();
    m_->viewProjectionMatrixInverse() = glm::inverse(m_->viewProjectionMatrix());
}

const glm::mat4 &RenderInfo::viewProjectionMatrix() const
{
    return m_->viewProjectionMatrix();
}

const glm::mat4 &RenderInfo::viewProjectionMatrixInverse() const
{
    return m_->viewProjectionMatrixInverse();
}

}
}

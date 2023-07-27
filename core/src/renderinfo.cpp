#include <core/renderinfo.h>

#include "renderinfoprivate.h"

namespace simplex
{
namespace core
{

RenderInfo::RenderInfo()
    : m_(std::make_unique<RenderInfoPrivate>())
{
    setFrameBuffer(nullptr);
    setViewport(glm::uvec4(0u, 0u, 1u, 1u));

    setViewMatrix(glm::mat4x4(1.f));
    setProjectionMatrix(glm::mat4x4(1.f));

    setGBufferMaps(nullptr, nullptr, nullptr);

    setFaceCulling(false);

    for (uint16_t i = 0; i < graphics::FrameBufferColorAttachmentsCount(); ++i) setColorMask(i, true);
    setDepthTest(true);
    setDepthMask(true);
    setStencilTest(false);
}

RenderInfo::~RenderInfo()
{
}

const std::shared_ptr<graphics::IFrameBuffer> RenderInfo::frameBuffer() const
{
    return m_->frameBuffer();
}

void RenderInfo::setFrameBuffer(std::shared_ptr<graphics::IFrameBuffer> value)
{
    m_->frameBuffer() = value;
}

const glm::uvec4 &RenderInfo::viewport() const
{
    return m_->viewport();
}

void RenderInfo::setViewport(const glm::uvec4 &vp)
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

const std::shared_ptr<const graphics::ITexture> &RenderInfo::gBufferColor0Map() const
{
    return m_->gBufferColor0Map();
}

const std::shared_ptr<const graphics::ITexture> &RenderInfo::gBufferColor1Map() const
{
    return m_->gBufferColor1Map();
}

const std::shared_ptr<const graphics::ITexture> &RenderInfo::gBufferDepthMap() const
{
    return m_->gBufferDepthMap();
}

void RenderInfo::setGBufferMaps(std::shared_ptr<const graphics::ITexture> color0Map,
                                std::shared_ptr<const graphics::ITexture> color1Map,
                                std::shared_ptr<const graphics::ITexture> depthMap)
{
    m_->gBufferColor0Map() = color0Map;
    m_->gBufferColor1Map() = color1Map;
    m_->gBufferDepthMap() = depthMap;
}

bool RenderInfo::faceCulling() const
{
    return m_->faceCulling();
}

graphics::FaceType RenderInfo::cullFaceType() const
{
    return m_->cullFaceType();
}

void RenderInfo::setFaceCulling(bool value, graphics::FaceType type)
{
    m_->faceCulling() = value;
    m_->cullFaceType() = type;
}

bool RenderInfo::colorMask(uint16_t index) const
{
    assert(index < graphics::FrameBufferColorAttachmentsCount());
    return m_->colorMaks()[index];
}

void RenderInfo::setColorMask(uint16_t index, bool value)
{
    assert(index < graphics::FrameBufferColorAttachmentsCount());
    m_->colorMaks()[index] = value;
}

bool RenderInfo::depthTest() const
{
    return m_->depthTest();
}

graphics::DepthFunc RenderInfo::depthFunc() const
{
    return m_->depthFunc();
}

void RenderInfo::setDepthTest(bool value, graphics::DepthFunc func)
{
    m_->depthTest() = value;
    m_->depthFunc() = func;
}

bool RenderInfo::depthMask() const
{
    return m_->depthMask();
}

void RenderInfo::setDepthMask(bool value)
{
    m_->depthMask() = value;
}

bool RenderInfo::stencilTest() const
{
    return m_->stencilTest();
}

void RenderInfo::setStencilTest(bool value)
{
    m_->stencilTest() = value;
}

}
}

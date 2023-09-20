#include <utils/logger.h>

#include <core/renderinfo.h>

#include "renderinfoprivate.h"

namespace simplex
{
namespace core
{

RenderInfo::RenderInfo()
    : m_(std::make_unique<RenderInfoPrivate>())
{
    m_->viewMatrixUniform() = makeUniform(glm::mat4(1.f));
    m_->projectionMatrixUniform() = makeUniform(glm::mat4(1.f));
    setViewMatrix(glm::mat4x4(1.f));
    setProjectionMatrix(glm::mat4x4(1.f));

    setGBufferMaps(nullptr, nullptr, nullptr);

    setFaceCulling(false);

    for (uint16_t i = 0; i < graphics::FrameBufferColorAttachmentsCount(); ++i) setColorMask(i, true);
    setDepthTest(true);
    setDepthMask(true);
    setStencilTest(false);
}

RenderInfo::~RenderInfo() = default;

const PUniform<glm::mat4> &RenderInfo::viewMatrixUniform() const
{
    return m_->viewMatrixUniform();
}

const PUniform<glm::mat4> &RenderInfo::viewMatrixInverseUniform() const
{
    return m_->viewMatrixInverseUniform();
}

const PUniform<glm::vec3> &RenderInfo::viewPositionUniform() const
{
    return m_->viewPositionUniform();
}

const PUniform<glm::vec3> &RenderInfo::viewXDirectionUniform() const
{
    return m_->viewXDirectionUniform();
}

const PUniform<glm::vec3> &RenderInfo::viewYDirectionUniform() const
{
    return m_->viewYDirectionUniform();
}

const PUniform<glm::vec3> &RenderInfo::viewZDirectionUniform() const
{
    return m_->viewZDirectionUniform();
}

void RenderInfo::setViewMatrix(const glm::mat4 &vm)
{
    m_->viewMatrixUniform() = makeUniform(vm);
    m_->viewMatrixInverseUniform() = makeUniform(glm::inverse(viewMatrixUniform()->data()));

    m_->viewProjectionMatrixUniform() = makeUniform(projectionMatrixUniform()->data() * viewMatrixUniform()->data());
    m_->viewProjectionMatrixInverseUniform() = makeUniform(glm::inverse(viewProjectionMatrixUniform()->data()));

    m_->viewPositionUniform() = makeUniform(glm::vec3(viewMatrixInverseUniform()->data() * glm::vec4(0.f, 0.f, 0.f, 1.f)));
    m_->viewXDirectionUniform() = makeUniform(glm::normalize(glm::vec3(viewMatrixInverseUniform()->data() * glm::vec4(1.f, 0.f, 0.f, 0.f))));
    m_->viewYDirectionUniform() = makeUniform(glm::normalize(glm::vec3(viewMatrixInverseUniform()->data() * glm::vec4(0.f, 1.f, 0.f, 0.f))));
    m_->viewZDirectionUniform() = makeUniform(glm::normalize(glm::vec3(viewMatrixInverseUniform()->data() * glm::vec4(0.f, 0.f, 1.f, 0.f))));
}

const PUniform<glm::mat4> &RenderInfo::projectionMatrixUniform() const
{
    return m_->projectionMatrixUniform();
}

const PUniform<glm::mat4> &RenderInfo::projectionMatrixInverseUniform() const
{
    return m_->projectionMatrixInverseUniform();
}

void RenderInfo::setProjectionMatrix(const glm::mat4 &pm)
{
    m_->projectionMatrixUniform() = makeUniform(pm);
    m_->projectionMatrixInverseUniform() = makeUniform(glm::inverse(projectionMatrixUniform()->data()));

    m_->viewProjectionMatrixUniform() = makeUniform(projectionMatrixUniform()->data() * viewMatrixUniform()->data());
    m_->viewProjectionMatrixInverseUniform() = makeUniform(glm::inverse(viewProjectionMatrixUniform()->data()));
}

const PUniform<glm::mat4> &RenderInfo::viewProjectionMatrixUniform() const
{
    return m_->viewProjectionMatrixUniform();
}

const PUniform<glm::mat4> &RenderInfo::viewProjectionMatrixInverseUniform() const
{
    return m_->viewProjectionMatrixInverseUniform();
}

const PUniform<graphics::PConstTexture> &RenderInfo::gBufferColor0MapUniform() const
{
    return m_->gBufferColor0MapUniform();
}

const PUniform<graphics::PConstTexture> &RenderInfo::gBufferColor1MapUniform() const
{
    return m_->gBufferColor1MapUniform();
}

const PUniform<graphics::PConstTexture> &RenderInfo::gBufferDepthMapUniform() const
{
    return m_->gBufferDepthMapUniform();
}

void RenderInfo::setGBufferMaps(const graphics::PConstTexture &color0Map,
                                const graphics::PConstTexture &color1Map,
                                const graphics::PConstTexture &depthMap)
{
    m_->gBufferColor0MapUniform() = makeUniform(color0Map);
    m_->gBufferColor1MapUniform() = makeUniform(color1Map);
    m_->gBufferDepthMapUniform() = makeUniform(depthMap);
}

const graphics::PConstBufferRange &RenderInfo::OITNodesBuffer() const
{
    return m_->OITDataBuffer();
}

void RenderInfo::setOITNodesBuffer(const graphics::PConstBufferRange &value)
{
    m_->OITDataBuffer() = value;
}

const PUniform<graphics::PConstBufferRange> &RenderInfo::OITNodesCounterUniform() const
{
    return m_->OITNodesCounterUniform();
}

void RenderInfo::setOITNodesCounter(const graphics::PConstBufferRange &value)
{
    m_->OITNodesCounterUniform() = makeUniform(value);
}

const PUniform<graphics::PConstImage> &RenderInfo::OITIndicesImageUniform() const
{
    return m_->OITIndicesImageUniform();
}

void RenderInfo::setOITIndicesImage(const graphics::PConstImage &value)
{
    m_->OITIndicesImageUniform() = makeUniform(value);
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
    if (index >= graphics::FrameBufferColorAttachmentsCount())
        LOG_CRITICAL << "Index must be less than " << graphics::FrameBufferColorAttachmentsCount();

    return m_->colorMasks()[index];
}

void RenderInfo::setColorMask(uint16_t index, bool value)
{
    if (index >= graphics::FrameBufferColorAttachmentsCount())
        LOG_CRITICAL << "Index must be less than " << graphics::FrameBufferColorAttachmentsCount();

    m_->colorMasks()[index] = value;
}

void RenderInfo::setColorMasks(bool value)
{
    for (auto &mask : m_->colorMasks())
        mask = value;
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

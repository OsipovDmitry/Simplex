#include <utils/logger.h>

#include <core/renderinfo.h>
#include <core/uniform.h>

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

    setGBufferTextures(nullptr, nullptr, nullptr, nullptr);
    setOITNodesBuffer(nullptr);
    setOITNodesCounter(nullptr);
    setOITDepthImage(nullptr);
    setOITIndicesImage(nullptr);
    setLightBufferColorTexture(nullptr);
    setFinalBufferColorTexture(nullptr);
}

RenderInfo::~RenderInfo() = default;

const PUniform<glm::mat4x4> &RenderInfo::viewMatrixUniform() const
{
    return m_->viewMatrixUniform();
}

const PUniform<glm::mat4x4> &RenderInfo::viewMatrixInverseUniform() const
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

const PUniform<glm::mat4x4> &RenderInfo::projectionMatrixUniform() const
{
    return m_->projectionMatrixUniform();
}

const PUniform<glm::mat4x4> &RenderInfo::projectionMatrixInverseUniform() const
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

const PUniform<glm::mat4x4> &RenderInfo::viewProjectionMatrixUniform() const
{
    return m_->viewProjectionMatrixUniform();
}

const PUniform<glm::mat4x4> &RenderInfo::viewProjectionMatrixInverseUniform() const
{
    return m_->viewProjectionMatrixInverseUniform();
}

const PUniform<graphics::PConstTexture> &RenderInfo::GBufferColor0TextureUniform() const
{
    return m_->GBufferColor0TextureUniform();
}

const PUniform<graphics::PConstTexture> &RenderInfo::GBufferColor1TextureUniform() const
{
    return m_->GBufferColor1TextureUniform();
}

const PUniform<graphics::PConstTexture> &RenderInfo::GBufferColor2TextureUniform() const
{
    return m_->GBufferColor2TextureUniform();
}

const PUniform<graphics::PConstTexture> &RenderInfo::GBufferDepthTextureUniform() const
{
    return m_->GBufferDepthTextureUniform();
}

void RenderInfo::setGBufferTextures(const graphics::PConstTexture &color0Map,
                                    const graphics::PConstTexture &color1Map,
                                    const graphics::PConstTexture &color2Map,
                                    const graphics::PConstTexture &depthMap)
{
    m_->GBufferColor0TextureUniform() = makeUniform(color0Map);
    m_->GBufferColor1TextureUniform() = makeUniform(color1Map);
    m_->GBufferColor2TextureUniform() = makeUniform(color2Map);
    m_->GBufferDepthTextureUniform() = makeUniform(depthMap);
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

const PUniform<graphics::PConstImage> &RenderInfo::OITDepthImageUniform() const
{
    return m_->OITDepthImageUniform();
}

void RenderInfo::setOITDepthImage(const graphics::PConstImage &value)
{
    m_->OITDepthImageUniform() = makeUniform(value);
}

const PUniform<graphics::PConstImage> &RenderInfo::OITIndicesImageUniform() const
{
    return m_->OITIndicesImageUniform();
}

void RenderInfo::setOITIndicesImage(const graphics::PConstImage &value)
{
    m_->OITIndicesImageUniform() = makeUniform(value);
}

const PUniform<graphics::PConstTexture> &RenderInfo::lightBufferColorTextureUniform() const
{
    return m_->lightBufferColorTextureUniform();
}

void RenderInfo::setLightBufferColorTexture(const graphics::PConstTexture &value)
{
    m_->lightBufferColorTextureUniform() = makeUniform(value);
}

const PUniform<graphics::PConstTexture> &RenderInfo::finalBufferColorTextureUniform() const
{
    return m_->finalBufferColorTextureUniform();
}

void RenderInfo::setFinalBufferColorTexture(const graphics::PConstTexture &value)
{
    m_->finalBufferColorTextureUniform() = makeUniform(value);
}

}
}

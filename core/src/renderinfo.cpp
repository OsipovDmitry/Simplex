#include <core/renderinfo.h>

#include "renderinfoprivate.h"

namespace simplex
{
namespace core
{

RenderInfo::RenderInfo()
    : m_(std::make_unique<RenderInfoPrivate>())
{
    setDefaultVertexPosition(glm::vec4(0.f, 0.f, 0.f, 1.f));
    setDefaultVertexNormal(glm::vec4(0.f, 0.f, 0.f, 0.f));
    setDefaultVertexTexCoord(glm::vec4(0.f, 0.f, 0.f, 0.f));
    setDefaultVertexBonesIDs(glm::uvec4(0u, 0u, 0u, 0u));
    setDefaultVertexBonesWeights(glm::vec4(1.f, 0.f, 0.f, 0.f));
    setDefaultVertexTangent(glm::vec4(1.f, 0.f, 0.f, 0.f));
    setDefaultVertexColor(glm::vec4(1.f, 1.f, 1.f, 1.f));

    setDefaultBaseColor(glm::vec4(1.f, 1.f, 1.f, 1.f));
    setDefaultMetalness(1.f);
    setDefaultRoughness(1.f);
    setDefaultBaseColorMap(nullptr);
    setDefaultMetalnessMap(nullptr);
    setDefaultRoughnessMap(nullptr);
    setDefaultNormalMap(nullptr);
    setDefaultBonesBuffer(nullptr);

    setFrameBuffer(nullptr);
    setViewport(glm::uvec4(0u, 0u, 1u, 1u));

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

const std::shared_ptr<graphics::IRenderProgram> RenderInfo::renderProgram() const
{
    return m_->renderProgram();
}

void RenderInfo::setRenderProgram(std::shared_ptr<graphics::IRenderProgram> value)
{
    m_->renderProgram() = value;
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

const PUniform<glm::uvec2> &RenderInfo::viewportSizeUniform() const
{
    return m_->viewportSizeUniform();
}

void RenderInfo::setViewport(const glm::uvec4 &vp)
{
    m_->viewport() = vp;
    m_->viewportSizeUniform() = makeUniform<glm::uvec2>(glm::uvec2(vp.z, vp.w));
}

const glm::vec4 &RenderInfo::defaultVertexPosition() const
{
    return m_->defaultVertexPosition();
}

void RenderInfo::setDefaultVertexPosition(const glm::vec4 &value)
{
    m_->defaultVertexPosition() = value;
}

const glm::vec4 &RenderInfo::defaultVertexNormal() const
{
    return m_->defaultVertexNormal();
}

void RenderInfo::setDefaultVertexNormal(const glm::vec4 &value)
{
    m_->defaultVertexNormal() = value;
}

const glm::vec4 &RenderInfo::defaultVertexTexCoord() const
{
    return m_->defaultVertexTexCoord();
}

void RenderInfo::setDefaultVertexTexCoord(const glm::vec4 &value)
{
    m_->defaultVertexTexCoord() = value;
}

const glm::uvec4 &RenderInfo::defaultVertexBonesIDs() const
{
    return m_->defaultVertexBonesIDs();
}

void RenderInfo::setDefaultVertexBonesIDs(const glm::uvec4 &value)
{
    m_->defaultVertexBonesIDs() = value;
}

const glm::vec4 &RenderInfo::defaultVertexBonesWeights() const
{
    return m_->defaultVertexBonesWeights();
}

void RenderInfo::setDefaultVertexBonesWeights(const glm::vec4 &value)
{
    m_->defaultVertexBonesWeights() = value;
}

const glm::vec4 &RenderInfo::defaultVertexTangent() const
{
    return m_->defaultVertexTangent();
}

void RenderInfo::setDefaultVertexTangent(const glm::vec4 &value)
{
    m_->defaultVertexTangent() = value;
}

const glm::vec4 &RenderInfo::defaultVertexColor() const
{
    return m_->defaultVertexColor();
}

void RenderInfo::setDefaultVertexColor(const glm::vec4 &value)
{
    m_->defaultVertexColor() = value;
}

const PUniform<glm::vec4> &RenderInfo::defaultBaseColorUniform() const
{
    return m_->defaultBaseColorUniform();
}

void RenderInfo::setDefaultBaseColor(const glm::vec4 &value)
{
    m_->defaultBaseColorUniform() = makeUniform<glm::vec4>(value);
}

const PUniform<float> &RenderInfo::defaultMetalnessUniform() const
{
    return m_->defaultMetallnessUniform();
}

void RenderInfo::setDefaultMetalness(float value)
{
    m_->defaultMetallnessUniform() = makeUniform<float>(value);
}

const PUniform<float> &RenderInfo::defaultRoughnessUniform() const
{
    return m_->defaultRoughnessUniform();
}

void RenderInfo::setDefaultRoughness(float value)
{
    m_->defaultRoughnessUniform() = makeUniform<float>(value);
}

const PUniform<graphics::PTexture> &RenderInfo::defaultBaseColorMapUniform() const
{
    return m_->defaultBaseColorMapUniform();
}

void RenderInfo::setDefaultBaseColorMap(const graphics::PTexture &value)
{
    m_->defaultBaseColorMapUniform() = makeUniform<graphics::PTexture>(value);
}

const PUniform<graphics::PTexture> &RenderInfo::defaultMetalnessMapUniform() const
{
    return m_->defaultBaseColorMapUniform();
}

void RenderInfo::setDefaultMetalnessMap(const graphics::PTexture &value)
{
    m_->defaultMetalnessMapUniform() = makeUniform<graphics::PTexture>(value);
}

const PUniform<graphics::PTexture> &RenderInfo::defaultRoughnessMapUniform() const
{
    return m_->defaultRoughnessMapUniform();
}

void RenderInfo::setDefaultRoughnessMap(const graphics::PTexture &value)
{
    m_->defaultRoughnessMapUniform() = makeUniform<graphics::PTexture>(value);
}

const PUniform<graphics::PTexture> &RenderInfo::defaultNormalMapUniform() const
{
    return m_->defaultNormalMapUniform();
}

void RenderInfo::setDefaultNormalMap(const graphics::PTexture &value)
{
    m_->defaultNormalMapUniform() = makeUniform<graphics::PTexture>(value);
}

const graphics::PBufferRange &RenderInfo::defaultBonesBuffer() const
{
    return m_->defaultBonesBuffer();
}

void RenderInfo::setDefaultBonesBuffer(const graphics::PBufferRange &value)
{
    m_->defaultBonesBuffer() = value;
}

const graphics::PBufferRange &RenderInfo::OITNodesBuffer() const
{
    return m_->OITDataBuffer();
}

void RenderInfo::setOITNodesBuffer(const graphics::PBufferRange &value)
{
    m_->OITDataBuffer() = value;
}

const PUniform<graphics::PBufferRange> &RenderInfo::OITNodesCounterUniform() const
{
    return m_->OITNodesCounterUniform();
}

void RenderInfo::setOITNodesCounter(const graphics::PBufferRange &value)
{
    m_->OITNodesCounterUniform() = makeUniform(value);
}

const PUniform<graphics::PImage> &RenderInfo::OITIndicesImageUniform() const
{
    return m_->OITIndicesImageUniform();
}

void RenderInfo::setOITIndicesImage(const graphics::PImage &value)
{
    m_->OITIndicesImageUniform() = makeUniform<graphics::PImage>(value);
}

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
    m_->viewMatrixUniform() = makeUniform<glm::mat4>(vm);
    m_->viewMatrixInverseUniform() = makeUniform<glm::mat4>(glm::inverse(viewMatrixUniform()->data()));

    m_->viewProjectionMatrixUniform() = makeUniform<glm::mat4>(projectionMatrixUniform()->data() * viewMatrixUniform()->data());
    m_->viewProjectionMatrixInverseUniform() = makeUniform<glm::mat4>(glm::inverse(viewProjectionMatrixUniform()->data()));

    m_->viewPositionUniform() = makeUniform<glm::vec3>(glm::vec3(viewMatrixInverseUniform()->data() * glm::vec4(0.f, 0.f, 0.f, 1.f)));
    m_->viewXDirectionUniform() = makeUniform<glm::vec3>(glm::vec3(viewMatrixInverseUniform()->data() * glm::vec4(1.f, 0.f, 0.f, 0.f)));
    m_->viewYDirectionUniform() = makeUniform<glm::vec3>(glm::vec3(viewMatrixInverseUniform()->data() * glm::vec4(0.f, 1.f, 0.f, 0.f)));
    m_->viewZDirectionUniform() = makeUniform<glm::vec3>(glm::vec3(viewMatrixInverseUniform()->data() * glm::vec4(0.f, 0.f, 1.f, 0.f)));
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
    m_->projectionMatrixUniform() = makeUniform<glm::mat4>(pm);
    m_->projectionMatrixInverseUniform() = makeUniform<glm::mat4>(glm::inverse(projectionMatrixUniform()->data()));

    m_->viewProjectionMatrixUniform() = makeUniform<glm::mat4>(projectionMatrixUniform()->data() * viewMatrixUniform()->data());
    m_->viewProjectionMatrixInverseUniform() = makeUniform<glm::mat4>(glm::inverse(viewProjectionMatrixUniform()->data()));

}

const PUniform<glm::mat4> &RenderInfo::viewProjectionMatrixUniform() const
{
    return m_->viewProjectionMatrixUniform();
}

const PUniform<glm::mat4> &RenderInfo::viewProjectionMatrixInverseUniform() const
{
    return m_->viewProjectionMatrixInverseUniform();
}

const PUniform<graphics::PTexture> &RenderInfo::gBufferColor0MapUniform() const
{
    return m_->gBufferColor0MapUniform();
}

const PUniform<graphics::PTexture> &RenderInfo::gBufferColor1MapUniform() const
{
    return m_->gBufferColor1MapUniform();
}

const PUniform<graphics::PTexture> &RenderInfo::gBufferDepthMapUniform() const
{
    return m_->gBufferDepthMapUniform();
}

void RenderInfo::setGBufferMaps(const graphics::PTexture &color0Map,
                                const graphics::PTexture &color1Map,
                                const graphics::PTexture &depthMap)
{
    m_->gBufferColor0MapUniform() = makeUniform<graphics::PTexture>(color0Map);
    m_->gBufferColor1MapUniform() = makeUniform<graphics::PTexture>(color1Map);
    m_->gBufferDepthMapUniform() = makeUniform<graphics::PTexture>(depthMap);
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
    return m_->colorMasks()[index];
}

void RenderInfo::setColorMask(uint16_t index, bool value)
{
    assert(index < graphics::FrameBufferColorAttachmentsCount());
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

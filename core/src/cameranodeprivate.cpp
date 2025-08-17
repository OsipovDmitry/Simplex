#include <utils/logger.h>

#include <core/cameranode.h>

#include "cameranodeprivate.h"
#include "framebufferhelpers.h"
#include "ssaoprivate.h"
#include "ssaodrawable.h"
#include "bilaterialblur.h"
#include "geometrybuffer.h"

namespace simplex
{
namespace core
{

CameraNodePrivate::CameraNodePrivate(
    CameraNode &cameraNode,
    const std::string &name,
    /*tmp*/const std::shared_ptr<graphics::IFrameBuffer>& defaultFrameBuffer)
    : NodePrivate(cameraNode, name)
    , m_isRenderingEnabled(true)
    , m_clipSpaceType(utils::ClipSpaceType::Perspective)
    , m_clipSpaceVerticalParam(glm::pi<float>() / 3.f)
    , m_clipSpace()
    , m_defaultFrameBuffer(defaultFrameBuffer)
{
}

CameraNodePrivate::~CameraNodePrivate() = default;

bool &CameraNodePrivate::isRenderingEnabled()
{
    return m_isRenderingEnabled;
}

std::shared_ptr<GeometryBuffer>& CameraNodePrivate::geometryBuffer()
{
    return m_geometryBuffer;
}

utils::ClipSpaceType &CameraNodePrivate::clipSpaceType()
{
    return m_clipSpaceType;
}

float &CameraNodePrivate::clipSpaceVerticalParam()
{
    return m_clipSpaceVerticalParam;
}

utils::ClipSpace &CameraNodePrivate::clipSpace()
{
    return m_clipSpace;
}

utils::Range &CameraNodePrivate::cullPlanesLimits()
{
    return m_cullPlanesLimits;
}

SSAO &CameraNodePrivate::ssao()
{
    return m_ssao;
}

std::shared_ptr<GFrameBufferTmp> &CameraNodePrivate::gFrameBuffer()
{
    return m_gFrameBuffer;
}

std::shared_ptr<SSAOFrameBuffer> &CameraNodePrivate::ssaoFrameBuffer()
{
    return m_ssaoFrameBuffer;
}

std::shared_ptr<BlurFrameBuffer> &CameraNodePrivate::ssaoBlurFrameBuffer()
{
    return m_ssaoBlurFrameBuffer;
}

std::shared_ptr<PostprocessFrameBuffer> &CameraNodePrivate::postprocessFrameBuffer()
{
    return m_postprocessFrameBuffer;
}

void CameraNodePrivate::resize(
    const std::shared_ptr<graphics::RendererBase> &graphicsRenderer,
    const std::shared_ptr<graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<GeometryBuffer>& geometryBuffer)
{
    const auto viewportSize = glm::max(glm::uvec2(1u), geometryBuffer->size());
    const float viewportAspectRatio = static_cast<float>(viewportSize[0u]) / static_cast<float>(viewportSize[1u]);

    switch (m_clipSpaceType)
    {
    case utils::ClipSpaceType::Ortho:
    {
        m_clipSpace = utils::ClipSpace::makeOrtho(viewportAspectRatio, m_clipSpaceVerticalParam);
        break;
    }
    case utils::ClipSpaceType::Perspective:
    {
        m_clipSpace = utils::ClipSpace::makePerspective(viewportAspectRatio, m_clipSpaceVerticalParam);
        break;
    }
    default:
    {
        LOG_CRITICAL << "Undefined ClipSpaceType";
        break;
    }
    }

    //tmp
    m_gFrameBuffer = std::make_shared<GFrameBufferTmp>(framebuffer, geometryBuffer);
    m_postprocessFrameBuffer = std::make_shared<PostprocessFrameBuffer>(m_defaultFrameBuffer);

    if (m_ssao.mode() != SSAOMode::Disabled)
    {
        if (!m_ssaoFrameBuffer)
            m_ssaoFrameBuffer = std::make_shared<SSAOFrameBuffer>(graphicsRenderer);

        if (!m_ssaoBlurFrameBuffer)
            m_ssaoBlurFrameBuffer = std::make_shared<BlurFrameBuffer>(graphicsRenderer);

        const auto SSAOViewportSize = glm::max(glm::uvec2(1u), glm::uvec2(glm::vec2(viewportSize) / ssao().viewportDenominator()));
        m_ssaoFrameBuffer->resize(graphicsRenderer, SSAOViewportSize, ssao().mode());
        m_ssaoBlurFrameBuffer->resize(graphicsRenderer, SSAOViewportSize, m_ssaoFrameBuffer->pixelInternalFormat());

        auto &ssaoPrivate = m_ssao.m();
        ssaoPrivate.drawable()->update(graphicsRenderer);
        //ssaoPrivate.blur()->update(graphicsRenderer);
    }
    else
    {
        m_ssaoFrameBuffer = nullptr;
        m_ssaoBlurFrameBuffer = nullptr;
    }

}

}
}

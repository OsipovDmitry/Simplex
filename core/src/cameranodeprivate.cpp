#include <utils/logger.h>

#include <core/cameranode.h>

#include "cameranodeprivate.h"

namespace simplex
{
namespace core
{

CameraNodePrivate::CameraNodePrivate(CameraNode &cameraNode, const std::string &name)
    : NodePrivate(cameraNode, name)
    , m_isRenderingEnabled(true)
    , m_clipSpaceType(utils::ClipSpaceType::Perspective)
    , m_clipSpaceVerticalParam(glm::pi<float>() / 3.f)
    , m_viewportAspectRatio(1.f)
    , m_clipSpace()
    , m_clusterMaxSize()
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

glm::uvec3& CameraNodePrivate::clusterMaxSize()
{
    return m_clusterMaxSize;
}

void CameraNodePrivate::resize(const glm::uvec2& size)
{
    const auto viewportSize = glm::max(glm::uvec2(1u), size);
    const auto viewportAspectRatio = static_cast<float>(viewportSize[0u]) / static_cast<float>(viewportSize[1u]);

    if (m_viewportAspectRatio != viewportAspectRatio)
    {
        m_viewportAspectRatio = viewportAspectRatio;
        updateClipSpace();
    }
}

void CameraNodePrivate::updateClipSpace()
{
    switch (m_clipSpaceType)
    {
    case utils::ClipSpaceType::Ortho:
    {
        m_clipSpace = utils::ClipSpace::makeOrtho(m_viewportAspectRatio, m_clipSpaceVerticalParam);
        break;
    }
    case utils::ClipSpaceType::Perspective:
    {
        m_clipSpace = utils::ClipSpace::makePerspective(m_viewportAspectRatio, m_clipSpaceVerticalParam);
        break;
    }
    default:
    {
        LOG_CRITICAL << "Undefined ClipSpaceType";
        break;
    }
    }
}

}
}

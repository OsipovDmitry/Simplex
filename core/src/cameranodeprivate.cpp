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
    , m_clipSpace()
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

void CameraNodePrivate::resize(const glm::uvec2& size)
{
    const auto viewportSize = glm::max(glm::uvec2(1u), size);
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
}

}
}

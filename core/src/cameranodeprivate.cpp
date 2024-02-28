#include "cameranodeprivate.h"

namespace simplex
{
namespace core
{

CameraNodePrivate::CameraNodePrivate(const std::string &name)
    : NodePrivate(name)
    , m_viewportSize(0u, 0u)
{
}

CameraNodePrivate::~CameraNodePrivate() = default;

std::shared_ptr<utils::AbstractClipSpace> &CameraNodePrivate::clipSpace()
{
    return m_clipSpace;
}

bool &CameraNodePrivate::isRenderingEnabled()
{
    return m_isRenderingEnabled;
}

glm::uvec2 &CameraNodePrivate::viewportSize()
{
    return m_viewportSize;
}

utils::Range &CameraNodePrivate::cullPlanesLimits()
{
    return m_cullPlanesLimits;
}

std::shared_ptr<GFrameBuffer> &CameraNodePrivate::gFrameBuffer()
{
    return m_gFrameBuffer;
}

std::shared_ptr<OITFrameBuffer> &CameraNodePrivate::oitFrameBuffer()
{
    return m_oitFrameBuffer;
}

std::shared_ptr<LightFrameBuffer> &CameraNodePrivate::lightFrameBuffer()
{
    return m_lightFrameBuffer;
}

std::shared_ptr<FinalFrameBuffer> &CameraNodePrivate::finalFrameBuffer()
{
    return m_finalFrameBuffer;
}

std::shared_ptr<PostprocessFrameBuffer> &CameraNodePrivate::postprocessFrameBuffer()
{
    return m_postprocessFrameBuffer;
}

}
}

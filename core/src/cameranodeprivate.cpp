#include <core/rendersurface.h>

#include "cameranodeprivate.h"

namespace simplex
{
namespace core
{

CameraNodePrivate::CameraNodePrivate(const std::string &name)
    : NodePrivate(name)
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

std::shared_ptr<RenderSurface> &CameraNodePrivate::renderSurface()
{
    return m_renderSurface;
}

std::shared_ptr<GSurface> &CameraNodePrivate::gSurface()
{
    return m_gSurface;
}

}
}

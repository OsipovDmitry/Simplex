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

std::shared_ptr<graphics::IFrameBuffer> &CameraNodePrivate::GFrameBuffer()
{
    return m_GFrameBuffer;
}

std::shared_ptr<graphics::IFrameBuffer> &CameraNodePrivate::OITFrameBuffer()
{
    return m_OITFrameBuffer;
}

std::shared_ptr<graphics::IFrameBuffer> &CameraNodePrivate::OITClearIndicesFrameBuffer()
{
    return m_OITClearIndicesFrameBuffer;
}

std::shared_ptr<graphics::IFrameBuffer> &CameraNodePrivate::OITSortNodesFrameBuffer()
{
    return m_OITSortNodesFrameBuffer;
}

std::shared_ptr<graphics::IFrameBuffer> &CameraNodePrivate::finalFrameBuffer()
{
    return m_finalFrameBuffer;
}

std::shared_ptr<graphics::IImage> &CameraNodePrivate::OITIndicesImage()
{
    return m_OITIndicesImage;
}

}
}

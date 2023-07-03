#include <core/viewport.h>

#include "cameranodeprivate.h"

namespace simplex
{
namespace core
{

std::shared_ptr<IGraphicsRenderer::VertexArray> CameraNodePrivate::s_cameraVertexArray;
std::shared_ptr<IGraphicsRenderer::FrameBuffer> CameraNodePrivate::s_defaultFrameBuffer;

CameraNodePrivate::CameraNodePrivate(const std::string &name)
    : NodePrivate(name)
    , m_viewport(std::make_shared<Viewport>())
    , m_frameBuffer(nullptr)
{
}

CameraNodePrivate::~CameraNodePrivate()
{
}

std::shared_ptr<utils::AbstractClipSpace> &CameraNodePrivate::clipSpace()
{
    return m_clipSpace;
}

bool &CameraNodePrivate::renderingEnabled()
{
    return m_renderingEnabled;
}

std::shared_ptr<Viewport> &CameraNodePrivate::viewport()
{
    return m_viewport;
}

std::shared_ptr<IGraphicsRenderer::FrameBuffer> &CameraNodePrivate::frameBuffer()
{
    return m_frameBuffer;
}

std::shared_ptr<ColoredDrawable> &CameraNodePrivate::cameraDrawable()
{
    return m_cameraDrawable;
}

std::shared_ptr<ColoredDrawable> &CameraNodePrivate::frustumDrawable()
{
    return m_frustumDrawable;
}

std::shared_ptr<IGraphicsRenderer::VertexArray> &CameraNodePrivate::cameraVertexArray()
{
    return s_cameraVertexArray;
}

std::shared_ptr<IGraphicsRenderer::FrameBuffer> &CameraNodePrivate::defaultFrameBuffer()
{
    return s_defaultFrameBuffer;
}

}
}

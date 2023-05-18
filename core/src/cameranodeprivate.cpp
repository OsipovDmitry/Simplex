#include "cameranodeprivate.h"

namespace simplex
{
namespace core
{

std::shared_ptr<IGraphicsRenderer::VertexArray> CameraNodePrivate::s_cameraVertexArray;

CameraNodePrivate::CameraNodePrivate(const std::string &name)
    : NodePrivate(name)
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

}
}

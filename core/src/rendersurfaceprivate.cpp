#include "rendersurfaceprivate.h"

namespace simplex
{
namespace core
{

RenderSurfacePrivate::RenderSurfacePrivate(std::weak_ptr<graphics::IRenderer> renderer,
                                           std::shared_ptr<graphics::IFrameBuffer> frameBuffer)
    : m_renderer(renderer)
    , m_frameBuffer(frameBuffer)
    , m_size(0u, 0u)
    , m_fixedSize(0u, 0u)
    , m_sizeScale(1.f)
    , m_isSizeFixed(false)
{
}

std::weak_ptr<graphics::IRenderer> &RenderSurfacePrivate::renderer()
{
    return m_renderer;
}

std::shared_ptr<graphics::IFrameBuffer> &RenderSurfacePrivate::frameBuffer()
{
    return m_frameBuffer;
}

glm::uvec2 &RenderSurfacePrivate::size()
{
    return m_size;
}

glm::uvec2 &RenderSurfacePrivate::fixedSize()
{
    return m_fixedSize;
}

float &RenderSurfacePrivate::sizeScale()
{
    return m_sizeScale;
}

bool &RenderSurfacePrivate::isSizeFixed()
{
    return m_isSizeFixed;
}


}
}

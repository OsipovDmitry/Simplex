#include <utils/glm/ext/vector_common.hpp>

#include <core/rendersurface.h>

#include "rendersurfaceprivate.h"

namespace simplex
{
namespace core
{

RenderSurface::~RenderSurface() = default;

std::weak_ptr<graphics::IRenderer> RenderSurface::renderer()
{
    return m_->renderer();
}

void RenderSurface::resize(const glm::uvec2 &size)
{
    const glm::uvec2 newSize = glm::max(
                m_->isSizeFixed() ? m_->fixedSize() : glm::uvec2(glm::vec2(size) * m_->sizeScale()),
                glm::uvec2(1u, 1u)
                );

    if (newSize != m_->size())
        doResize(newSize);

    m_->size() = newSize;
}

const glm::uvec2 &RenderSurface::viewportSize() const
{
    return m_->size();
}

bool RenderSurface::isSizeFixed() const
{
    return m_->isSizeFixed();
}

float RenderSurface::sizeScale() const
{
    return m_->sizeScale();
}

const glm::uvec2 &RenderSurface::fixedSize() const
{
    return m_->fixedSize();
}

void RenderSurface::setFixedSize(const glm::vec2& value)
{
    m_->isSizeFixed() = true;
    m_->fixedSize() = value;
}

void RenderSurface::setFloatingSize(float value)
{
    m_->isSizeFixed() = false;
    m_->sizeScale() = value;
}

std::shared_ptr<graphics::IFrameBuffer> RenderSurface::frameBuffer()
{
    return m_->frameBuffer();
}

std::shared_ptr<const graphics::IFrameBuffer> RenderSurface::frameBuffer() const
{
    return m_->frameBuffer();
}

RenderSurface::RenderSurface(std::weak_ptr<graphics::IRenderer> renderer,
                             std::shared_ptr<graphics::IFrameBuffer> frameBuffer)
    : RenderSurface(std::make_unique<RenderSurfacePrivate>(renderer, frameBuffer))
{
}

RenderSurface::RenderSurface(std::unique_ptr<RenderSurfacePrivate> renderSurfacePrivate)
    : m_(std::move(renderSurfacePrivate))
{
    assert(m_->frameBuffer());
    setFloatingSize();
}





}
}


#include "pointshadowprivate.h"
#include "framebufferhelpers.h"

namespace simplex
{
namespace core
{

PointShadowPrivate::PointShadowPrivate()
    : ShadowPrivate()
{
}

PointShadowPrivate::~PointShadowPrivate() = default;

std::shared_ptr<ShadowFrameBuffer> PointShadowPrivate::createShadowFrameBuffer(const std::shared_ptr<graphics::RendererBase> &graphicsRenderer) const
{
    return std::make_shared<ShadowFrameBufferCube>(graphicsRenderer);
}

}
}

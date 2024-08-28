#include "directionalshadowprivate.h"
#include "framebufferhelpers.h"

namespace simplex
{
namespace core
{

DirectionalShadowPrivate::DirectionalShadowPrivate()
    : ShadowPrivate()
{
}

DirectionalShadowPrivate::~DirectionalShadowPrivate() = default;

std::shared_ptr<ShadowFrameBuffer> DirectionalShadowPrivate::createShadowFrameBuffer(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer) const
{
    return std::make_shared<ShadowFrameBuffer2D>(graphicsRenderer);
}

}
}

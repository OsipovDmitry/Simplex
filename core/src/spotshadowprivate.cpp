#include "spotshadowprivate.h"
#include "framebufferhelpers.h"

namespace simplex
{
namespace core
{

SpotShadowPrivate::SpotShadowPrivate()
    : ShadowPrivate()
{
}

SpotShadowPrivate::~SpotShadowPrivate() = default;

std::shared_ptr<ShadowFrameBuffer> SpotShadowPrivate::createShadowFrameBuffer(const std::shared_ptr<graphics::RendererBase> &graphicsRenderer) const
{
    return std::make_shared<ShadowFrameBuffer2D>(graphicsRenderer);
}

}
}

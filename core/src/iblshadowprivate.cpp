#include "iblshadowprivate.h"

namespace simplex
{
namespace core
{

IBLShadowPrivate::IBLShadowPrivate()
    : ShadowPrivate()
{
}

IBLShadowPrivate::~IBLShadowPrivate() = default;

std::shared_ptr<ShadowFrameBuffer> IBLShadowPrivate::createShadowFrameBuffer(const std::shared_ptr<graphics::IRenderer> &) const
{
    return nullptr;
}

}
}

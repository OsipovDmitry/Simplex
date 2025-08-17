#include <core/uniform.h>

#include "shadowrenderinfo.h"

namespace simplex
{
namespace core
{

ShadowRenderInfo::ShadowRenderInfo()
    : StateSet()
{
}

ShadowRenderInfo::~ShadowRenderInfo() = default;

void ShadowRenderInfo::setViewportSize(const glm::uvec2& value)
{
    getOrCreateUniform(core::UniformId::ViewportSize) = makeUniform(value);
}

void ShadowRenderInfo::setLayeredShadowMatricesBuffer(const graphics::PConstBuffer &value)
{
    getOrCreateSSBO(SSBOId::LayeredShadowMatrices) = graphics::BufferRange::create(value);
}

}
}

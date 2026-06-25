#include "shadowedlightnodeprivate.h"

#include <core/shadowedlightnode.h>

namespace simplex
{
namespace core
{

ShadowedLightNodePrivate::ShadowedLightNodePrivate(ShadowedLightNode& shadowedLightNode, const std::string& name, LightType type)
    : ColoredLightNodePrivate(shadowedLightNode, name, type)
{
}

ShadowedLightNodePrivate::~ShadowedLightNodePrivate() = default;

std::shared_ptr<Shadow>& ShadowedLightNodePrivate::shadow()
{
    return m_shadow;
}

uint32_t& ShadowedLightNodePrivate::shadowMapSize()
{
    return m_shadowMapSize;
}

utils::Range& ShadowedLightNodePrivate::shadowCullPlanesLimits()
{
    return m_shadowCullPlanesLimits;
}

bool& ShadowedLightNodePrivate::isVolumetricScatteringEnabled()
{
    return m_isVolumetricScatteringEnabled;
}

} // namespace core
} // namespace simplex

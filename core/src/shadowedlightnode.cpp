#include <utils/logger.h>

#include <core/settings.h>
#include <core/shadowedlightnode.h>

#include "shadow.h"
#include "shadowedlightnodeprivate.h"

namespace simplex
{
namespace core
{

ShadowedLightNode::~ShadowedLightNode() = default;

std::shared_ptr<DirectionalLightNode> ShadowedLightNode::asDirectionalLightNode()
{
    return nullptr;
}

std::shared_ptr<const DirectionalLightNode> ShadowedLightNode::asDirectionalLightNode() const
{
    return nullptr;
}

std::shared_ptr<PointLightNode> ShadowedLightNode::asPointLightNode()
{
    return nullptr;
}

std::shared_ptr<const PointLightNode> ShadowedLightNode::asPointLightNode() const
{
    return nullptr;
}

std::shared_ptr<SpotLightNode> ShadowedLightNode::asSpotLightNode()
{
    return nullptr;
}

std::shared_ptr<const SpotLightNode> ShadowedLightNode::asSpotLightNode() const
{
    return nullptr;
}

bool ShadowedLightNode::isShadingEnabled() const
{
    return m().shadow() != nullptr;
}

void ShadowedLightNode::setShadingEnabled(bool value)
{
    auto& mPrivate = m();

    if (auto& shadow = mPrivate.shadow(); !value)
    {
        shadow.reset();
    }
    else
    {
        if (!shadow) shadow = std::make_shared<Shadow>(*this);
        shadow->onChanged();
    }

    mPrivate.onChanged();
}

uint32_t ShadowedLightNode::shadowMapSize() const
{
    return m().shadowMapSize();
}

void ShadowedLightNode::setShadowMapSize(uint32_t value)
{
    auto& mPrivate = m();
    mPrivate.shadowMapSize() = value;
    if (auto& shadow = mPrivate.shadow()) shadow->onChanged();
}

const utils::Range& ShadowedLightNode::shadowCullPlanesLimits() const
{
    return m().shadowCullPlanesLimits();
}

void ShadowedLightNode::setShadowCullPlanesLimits(const utils::Range& value)
{
    if (value.nearValue() <= 0.f) LOG_CRITICAL << "Cull near plane limit must be greater than 0.0";
    if (value.farValue() <= value.nearValue())
        LOG_CRITICAL << "Cull far plane limit must be greater than cull near plane value limit";

    auto& mPrivate = m();
    mPrivate.shadowCullPlanesLimits() = value;
    if (auto& shadow = mPrivate.shadow()) shadow->onChanged();
}

bool ShadowedLightNode::isVolumetricScatteringEnabled() const
{
    return m().isVolumetricScatteringEnabled();
}

void ShadowedLightNode::setVolumetricScatteringEnabled(bool value)
{
    auto& mPrivate = m();
    mPrivate.isVolumetricScatteringEnabled() = value;
    mPrivate.onChanged();
}

ShadowedLightNode::ShadowedLightNode(std::unique_ptr<ShadowedLightNodePrivate> shadowedLightNodePrivate)
    : ColoredLightNode(std::move(shadowedLightNodePrivate))
{
    setShadingEnabled(false);
    setVolumetricScatteringEnabled(false);

    const auto& shadowSettings = settings::Settings::instance().graphics().shadow();

    setShadowMapSize(shadowSettings.mapSize());
    setShadowCullPlanesLimits(shadowSettings.cullPlaneLimits());
}

} // namespace core
} // namespace simplex

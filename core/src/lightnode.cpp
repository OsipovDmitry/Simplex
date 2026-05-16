#include <utils/logger.h>

#include <core/lightnode.h>
#include <core/settings.h>

#include "lightnodeprivate.h"
#include "shadow.h"

namespace simplex
{
namespace core
{

LightNode::~LightNode() = default;

std::shared_ptr<LightNode> LightNode::asLightNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<LightNode>(wp.lock());
}

std::shared_ptr<const LightNode> LightNode::asLightNode() const
{
    return const_cast<LightNode*>(this)->asLightNode();
}

LightType LightNode::type() const
{
    return m().type();
}

std::shared_ptr<PointLightNode> LightNode::asPointLightNode()
{
    return nullptr;
}

std::shared_ptr<const PointLightNode> LightNode::asPointLightNode() const
{
    return nullptr;
}

std::shared_ptr<SpotLightNode> LightNode::asSpotLightNode()
{
    return nullptr;
}

std::shared_ptr<const SpotLightNode> LightNode::asSpotLightNode() const
{
    return nullptr;
}

std::shared_ptr<DirectionalLightNode> LightNode::asDirectionalLightNode()
{
    return nullptr;
}

std::shared_ptr<const DirectionalLightNode> LightNode::asDirectionalLightNode() const
{
    return nullptr;
}

std::shared_ptr<ImageBasedLightNode> LightNode::asIBLLightNode()
{
    return nullptr;
}

std::shared_ptr<const ImageBasedLightNode> LightNode::asIBLLightNode() const
{
    return nullptr;
}

std::shared_ptr<AmbientLightNode> LightNode::asAmbientLightNode()
{
    return nullptr;
}

std::shared_ptr<const AmbientLightNode> LightNode::asAmbientLightNode() const
{
    return nullptr;
}

bool LightNode::isLightingEnabled() const
{
    return m().isLightingEnabled();
}

void LightNode::setLightingEnabled(bool value)
{
    auto& mPrivate = m();
    mPrivate.isLightingEnabled() = value;
    mPrivate.onChanged();
}

bool LightNode::isShadingEnabled() const
{
    return m().shadow() != nullptr;
}

void LightNode::setShadingEnabled(bool value)
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

uint32_t LightNode::shadowMapSize() const
{
    return m().shadowMapSize();
}

void LightNode::setShadowMapSize(uint32_t value)
{
    auto& mPrivate = m();
    mPrivate.shadowMapSize() = value;
    if (auto& shadow = mPrivate.shadow()) shadow->onChanged();
}

const utils::Range& LightNode::shadowCullPlanesLimits() const
{
    return m().shadowCullPlanesLimits();
}

void LightNode::setShadowCullPlanesLimits(const utils::Range& value)
{
    if (value.nearValue() <= 0.f) LOG_CRITICAL << "ZNear must be greater than 0.0";
    if (value.farValue() <= value.nearValue()) LOG_CRITICAL << "ZFar must be greater than Znear";

    auto& mPrivate = m();
    mPrivate.shadowCullPlanesLimits() = value;
    if (auto& shadow = mPrivate.shadow()) shadow->onChanged();
}

LightNode::LightNode(std::unique_ptr<LightNodePrivate> lightNodePrivate)
    : Node(std::move(lightNodePrivate))
{
    setLightingEnabled(true);
    setShadingEnabled(false);

    const auto& graphicsSettings = settings::Settings::instance().graphics();
    const auto& shadowSettings = graphicsSettings.shadow();

    setShadowMapSize(shadowSettings.mapSize());
    setShadowCullPlanesLimits(graphicsSettings.cullPlaneLimits());
}

} // namespace core
} // namespace simplex

#include "directionallightnodeprivate.h"

#include <core/directionallightnode.h>
#include <core/graphicsrendererbase.h>

#include "scenedata.h"

namespace simplex
{
namespace core
{

DirectionalLightNodePrivate::DirectionalLightNodePrivate(DirectionalLightNode& directionalLightNode, const std::string& name)
    : ShadowedLightNodePrivate(directionalLightNode, name, LightType::Directional)
{
}

DirectionalLightNodePrivate::~DirectionalLightNodePrivate() = default;

uint32_t DirectionalLightNodePrivate::shadowLayersCount() const
{
    return m_shadowCascadesCount;
}

uint32_t& DirectionalLightNodePrivate::shadowCascadesCount()
{
    return m_shadowCascadesCount;
}

std::shared_ptr<LightHandler> DirectionalLightNodePrivate::createLightInSceneData(SceneData& sceneData) const
{
    return sceneData.addDirectionalLight(
        d().globalTransform(), m_isLightingEnabled, m_color, m_shadow, m_isVolumetricScatteringEnabled);
}

void DirectionalLightNodePrivate::updateLightInSceneData(LightHandler& handler) const
{
    if (auto sceneData = handler.sceneData().lock())
        sceneData->onDirectionalLightChanged(
            handler, d().globalTransform(), m_isLightingEnabled, m_color, m_shadow, m_isVolumetricScatteringEnabled);
}

} // namespace core
} // namespace simplex

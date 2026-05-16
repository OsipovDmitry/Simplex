#include "directionallightnodeprivate.h"

#include <core/directionallightnode.h>
#include <core/graphicsrendererbase.h>

#include "scenedata.h"

namespace simplex
{
namespace core
{

DirectionalLightNodePrivate::DirectionalLightNodePrivate(DirectionalLightNode& directionalLightNode, const std::string& name)
    : LightNodePrivate(directionalLightNode, name, LightType::Directional)
{
}

DirectionalLightNodePrivate::~DirectionalLightNodePrivate() = default;

void DirectionalLightNodePrivate::updateLightInSceneData(SceneData& sceneData, utils::IDsGeneratorT<size_t>::value_type lightID)
    const
{
    sceneData.onDirectionalLightChanged(lightID, d().globalTransform(), m_isLightingEnabled, m_color, m_shadow);
}

glm::vec3& DirectionalLightNodePrivate::color()
{
    return m_color;
}

uint32_t& DirectionalLightNodePrivate::shadowCascadesCount()
{
    return m_shadowCascadesCount;
}

uint32_t DirectionalLightNodePrivate::shadowLayersCount() const
{
    return m_shadowCascadesCount;
}

std::shared_ptr<LightHandler> DirectionalLightNodePrivate::createLightInSceneData(SceneData& sceneData) const
{
    return sceneData.addDirectionalLight(d().globalTransform(), m_isLightingEnabled, m_color, m_shadow);
}

} // namespace core
} // namespace simplex

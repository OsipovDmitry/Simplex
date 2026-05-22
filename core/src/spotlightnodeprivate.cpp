#include "spotlightnodeprivate.h"

#include <core/graphicsrendererbase.h>
#include <core/spotlightnode.h>

#include "scenedata.h"

namespace simplex
{
namespace core
{

SpotLightNodePrivate::SpotLightNodePrivate(SpotLightNode& spotLightNode, const std::string& name)
    : LightNodePrivate(spotLightNode, name, LightType::Spot)
{
}

SpotLightNodePrivate::~SpotLightNodePrivate() = default;

uint32_t SpotLightNodePrivate::shadowLayersCount() const
{
    return 1u;
}

glm::vec3& SpotLightNodePrivate::color()
{
    return m_color;
}

glm::vec2& SpotLightNodePrivate::radiuses()
{
    return m_radiuses;
}

glm::vec2& SpotLightNodePrivate::halfAngles()
{
    return m_halfAngles;
}

std::shared_ptr<LightHandler> SpotLightNodePrivate::createLightInSceneData(SceneData& sceneData) const
{
    return sceneData.addSpotLight(d().globalTransform(), m_isLightingEnabled, m_color, m_radiuses, m_halfAngles, m_shadow);
}

void SpotLightNodePrivate::updateLightInSceneData(LightHandler& handler) const
{
    if (auto sceneData = handler.sceneData().lock())
        sceneData->onSpotLightChanged(
            handler, d().globalTransform(), m_isLightingEnabled, m_color, m_radiuses, m_halfAngles, m_shadow);
}

} // namespace core
} // namespace simplex

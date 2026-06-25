#include "pointlightnodeprivate.h"

#include <core/graphicsrendererbase.h>
#include <core/pointlightnode.h>

#include "scenedata.h"

namespace simplex
{
namespace core
{

PointLightNodePrivate::PointLightNodePrivate(PointLightNode& pointLightNode, const std::string& name)
    : ShadowedLightNodePrivate(pointLightNode, name, LightType::Point)
{
}

PointLightNodePrivate::~PointLightNodePrivate() = default;

uint32_t PointLightNodePrivate::shadowLayersCount() const
{
    return 6u;
}

utils::Range& PointLightNodePrivate::radiuses()
{
    return m_radiuses;
}

std::shared_ptr<LightHandler> PointLightNodePrivate::createLightInSceneData(SceneData& sceneData) const
{
    return sceneData.addPointLight(
        d().globalTransform(), m_isLightingEnabled, m_color, m_radiuses, m_shadow, m_isVolumetricScatteringEnabled);
}

void PointLightNodePrivate::updateLightInSceneData(LightHandler& handler) const
{
    if (auto sceneData = handler.sceneData().lock())
        sceneData->onPointLightChanged(
            handler, d().globalTransform(), m_isLightingEnabled, m_color, m_radiuses, m_shadow, m_isVolumetricScatteringEnabled);
}

} // namespace core
} // namespace simplex

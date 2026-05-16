#include "pointlightnodeprivate.h"

#include <core/graphicsrendererbase.h>
#include <core/pointlightnode.h>

#include "scenedata.h"

namespace simplex
{
namespace core
{

PointLightNodePrivate::PointLightNodePrivate(PointLightNode& pointLightNode, const std::string& name)
    : LightNodePrivate(pointLightNode, name, LightType::Point)
{
}

PointLightNodePrivate::~PointLightNodePrivate() = default;

uint32_t PointLightNodePrivate::shadowLayersCount() const
{
    return 6u;
}

glm::vec3& PointLightNodePrivate::color()
{
    return m_color;
}

glm::vec2& PointLightNodePrivate::radiuses()
{
    return m_radiuses;
}

std::shared_ptr<LightHandler> PointLightNodePrivate::createLightInSceneData(SceneData& sceneData) const
{
    return sceneData.addPointLight(d().globalTransform(), m_isLightingEnabled, m_color, m_radiuses, m_shadow);
}

void PointLightNodePrivate::updateLightInSceneData(SceneData& sceneData, utils::IDsGeneratorT<size_t>::value_type lightID) const
{
    sceneData.onPointLightChanged(lightID, d().globalTransform(), m_isLightingEnabled, m_color, m_radiuses, m_shadow);
}

} // namespace core
} // namespace simplex

#include "ambientlightnodeprivate.h"

#include <core/ambientlightnode.h>

#include "scenedata.h"

namespace simplex
{
namespace core
{

AmbientLightNodePrivate::AmbientLightNodePrivate(AmbientLightNode& ambientLightNode, const std::string& name)
    : LightNodePrivate(ambientLightNode, name, LightType::Ambient)
    , m_color(0.f)
{
}

AmbientLightNodePrivate::~AmbientLightNodePrivate() = default;

uint32_t AmbientLightNodePrivate::shadowLayersCount() const
{
    return 0u;
}

glm::vec3& AmbientLightNodePrivate::color()
{
    return m_color;
}

std::shared_ptr<LightHandler> AmbientLightNodePrivate::createLightInSceneData(SceneData& sceneData) const
{
    return sceneData.addAmbientLight(m_isLightingEnabled, m_color);
}

void AmbientLightNodePrivate::updateLightInSceneData(LightHandler& handler) const
{
    if (auto sceneData = handler.sceneData().lock()) sceneData->onAmbientLightChanged(handler, m_isLightingEnabled, m_color);
}

} // namespace core
} // namespace simplex

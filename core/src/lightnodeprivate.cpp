#include "lightnodeprivate.h"

#include <core/scene.h>

#include "scenedata.h"
#include "sceneprivate.h"

namespace simplex
{
namespace core
{

LightNodePrivate::LightNodePrivate(LightNode& lightNode, const std::string& name, LightType type)
    : NodePrivate(lightNode, name)
    , m_type(type)
    , m_isLightingEnabled(true)
{
}

LightNodePrivate::~LightNodePrivate() = default;

void LightNodePrivate::onAfterTransformChanged()
{
    onChanged();
}

void LightNodePrivate::onAttachToScene(const std::shared_ptr<Scene>& scene)
{
    if (auto& sceneData = scene->m().sceneData())
    {
        m_lightHandler = createLightInSceneData(*sceneData);
    }
}

void LightNodePrivate::onDetachFromScene(const std::shared_ptr<Scene>&)
{
    m_lightHandler.reset();
}

LightType& LightNodePrivate::type()
{
    return m_type;
}

bool& LightNodePrivate::isLightingEnabled()
{
    return m_isLightingEnabled;
}

void LightNodePrivate::onChanged()
{
    if (m_lightHandler) updateLightInSceneData(*m_lightHandler);
}

} // namespace core
} // namespace simplex

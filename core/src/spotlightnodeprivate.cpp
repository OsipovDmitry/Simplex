#include <utils/logger.h>

#include <core/igraphicswidget.h>
#include <core/graphicsengine.h>
#include <core/graphicsrendererbase.h>
#include <core/spotlightnode.h>
#include <core/scene.h>

#include "graphicsengineprivate.h"
#include "spotlightnodeprivate.h"
#include "sceneprivate.h"

namespace simplex
{
namespace core
{

static inline float extendedRadius(float value)
{
    return value * 1.15f;
}

SpotLightNodePrivate::SpotLightNodePrivate(SpotLightNode &spotLightNode, const std::string &name)
    : LightNodePrivate(spotLightNode, name, LightType::Spot)
{
}

SpotLightNodePrivate::~SpotLightNodePrivate() = default;

void SpotLightNodePrivate::doAfterTransformChanged()
{
    changeInSceneData();
}

void SpotLightNodePrivate::doAttachToScene(const std::shared_ptr<Scene>& scene)
{
    if (auto& sceneData = scene->m().sceneData())
        addToSceneData(sceneData);
}

void SpotLightNodePrivate::doDetachFromScene(const std::shared_ptr<Scene>&)
{
    removeFromSceneData();
}

glm::vec3& SpotLightNodePrivate::color()
{
    return m_color;
}

glm::vec2& SpotLightNodePrivate::radiuses()
{
    return m_radiuses;
}

glm::vec2 &SpotLightNodePrivate::halfAngles()
{
    return m_halfAngles;
}

void SpotLightNodePrivate::addToSceneData(const std::shared_ptr<SceneData>&sceneData)
{
    if (!sceneData)
    {
        LOG_ERROR << "Scene data can't be nullptr";
        return;
    }

    m_handler = sceneData->addSpotLight(d().globalTransform(), doAreaScale(), m_color, m_radiuses, m_halfAngles);
}

void SpotLightNodePrivate::removeFromSceneData()
{
    m_handler.reset();
}

void SpotLightNodePrivate::changeInSceneData()
{
    if (m_handler)
        if (auto sceneData = m_handler->sceneData().lock())
            sceneData->onSpotLightChanged(m_handler->ID(), d().globalTransform(), doAreaScale(), m_color, m_radiuses, m_halfAngles);
}

std::shared_ptr<SpotLightHandler>& SpotLightNodePrivate::handler()
{
    return m_handler;
}

LightNodePrivate::ShadowTransform SpotLightNodePrivate::doShadowTransform(const utils::Frustum::Points &cameraFrustumPoints)
{
    static const float s_nearPlaneZ = -1.f;

    auto &dPublic = d();

    const auto viewTransform = dPublic.globalTransform().inverted();

    utils::BoundingBoxT<2u, float> cameraFrustumBB;
    for (const auto &[index, corner] : cameraFrustumPoints)
    {
        auto cornerInShadowSpace = viewTransform * corner;
        cornerInShadowSpace.z = glm::min(cornerInShadowSpace.z, s_nearPlaneZ);
        cameraFrustumBB += cornerInShadowSpace * s_nearPlaneZ / cornerInShadowSpace.z;
    }

    const float t = glm::tan(halfAngles()[1u]);
    const utils::BoundingBoxT<2u, float> lightFrustumBB({ glm::vec2(-t, -t), glm::vec2(t, t) });
    const auto shadowBB = cameraFrustumBB * lightFrustumBB;
    const auto clipSpace = utils::ClipSpace::makePerspective(shadowBB.minPoint().x, shadowBB.maxPoint().x,
                                                             shadowBB.minPoint().y, shadowBB.maxPoint().y);

    const auto numLayers = numLayeredShadowMatrices();

    ShadowTransform result;
    result.frustumViewTransform = viewTransform;
    result.frustumClipSpace = clipSpace;
    result.layeredViewTransforms.resize(numLayers);
    for (uint32_t i = 0; i < numLayers; ++i)
        result.layeredViewTransforms[i] = viewTransform;
    result.clipSpase = clipSpace;
    result.cullPlanesLimits = shadow().cullPlanesLimits() * utils::Range(0.f, dPublic.radiuses()[1u]);
    return result;
}

glm::mat4x4 SpotLightNodePrivate::doAreaMatrix()
{
    auto &dPublic = d();
    return glm::scale(glm::mat4x4(1.f), glm::vec3(extendedRadius(dPublic.radiuses()[1u])) * glm::vec3(glm::vec2(glm::tan(dPublic.halfAngles()[1u])), 1.f));
}

utils::BoundingBox SpotLightNodePrivate::doAreaBoundingBox()
{
    auto graphicsRenderer = graphics::RendererBase::current();
    if (!graphicsRenderer)
        LOG_CRITICAL << "Graphics renderer can't be nullptr";

    auto graphicsWidget = graphicsRenderer->widget();
    if (!graphicsWidget)
        LOG_CRITICAL << "Graphics widget can't be nullptr";

    auto graphicsEngine = graphicsWidget->graphicsEngine();
    if (!graphicsEngine)
        LOG_CRITICAL << "Graphics engine can't be nullptr";

    return areaMatrix() * utils::BoundingBox(); //graphicsEngine->m().spotLightAreaBoundingBox();
}

glm::vec3 SpotLightNodePrivate::doAreaScale() const
{
    return glm::vec3(extendedRadius(m_radiuses[1u])) * glm::vec3(glm::vec2(glm::tan(m_halfAngles[1u])), 1.f);
}

}
}

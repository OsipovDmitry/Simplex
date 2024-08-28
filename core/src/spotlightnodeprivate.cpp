#include <utils/logger.h>

#include <core/applicationbase.h>
#include <core/graphicsengine.h>
#include <core/spotlightnode.h>

#include "graphicsengineprivate.h"
#include "spotlightnodeprivate.h"
#include "spotshadowprivate.h"

namespace simplex
{
namespace core
{

inline float extendedRadius(float value)
{
    return value * 1.15f;
}

SpotLightNodePrivate::SpotLightNodePrivate(SpotLightNode &spotLightNode, const std::string &name)
    : LightNodePrivate(spotLightNode, name, std::make_unique<SpotShadowPrivate>())
{
}

SpotLightNodePrivate::~SpotLightNodePrivate() = default;

glm::vec2 &SpotLightNodePrivate::halfAngles()
{
    return m_halfAngles;
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

    ShadowTransform result;
    result.frustumViewTransform = viewTransform;
    result.frustumClipSpace = clipSpace;
    result.layeredViewTransforms = { viewTransform };
    result.clipSpase = clipSpace;
    result.cullPlanesLimits = m_shadow.cullPlanesLimits() * utils::Range(0.f, dPublic.radiuses()[1u]);
    return result;
}

glm::mat4x4 SpotLightNodePrivate::doAreaMatrix()
{
    auto &dPublic = d();
    return glm::scale(glm::mat4x4(1.f), glm::vec3(extendedRadius(dPublic.radiuses()[1u])) * glm::vec3(glm::vec2(glm::tan(dPublic.halfAngles()[1u])), 1.f));
}

utils::BoundingBox SpotLightNodePrivate::doAreaBoundingBox()
{
    auto app = ApplicationBase::currentApplication();
    if (!app)
        LOG_CRITICAL << "Application can't be nullptr";

    auto graphicsEngine = app->findEngine<GraphicsEngine>("");
    if (!graphicsEngine)
        LOG_CRITICAL << "Graphics engine can't be nullptr";

    return areaMatrix() * graphicsEngine->m().spotLightAreaBoundingBox();
}

}
}

#include <utils/logger.h>

#include <core/applicationbase.h>
#include <core/graphicsengine.h>
#include <core/pointlightnode.h>

#include "graphicsengineprivate.h"
#include "pointlightnodeprivate.h"
#include "pointshadowprivate.h"

namespace simplex
{
namespace core
{

inline float extendedRadius(float value)
{
    return value * 1.15f;
}

PointLightNodePrivate::PointLightNodePrivate(PointLightNode &pointLightNode, const std::string &name)
    : LightNodePrivate(pointLightNode, name, std::make_unique<PointShadowPrivate>())
{
}

PointLightNodePrivate::~PointLightNodePrivate() = default;

LightNodePrivate::ShadowTransform PointLightNodePrivate::doShadowTransform(const utils::Frustum::Points&)
{
    static const std::array<utils::Transform, 6u> s_layeredLookAt {
        utils::Transform::makeLookAt(glm::vec3(0.f), glm::vec3( 1.f, 0.f, 0.f), glm::vec3(0.f,-1.f, 0.f)),
        utils::Transform::makeLookAt(glm::vec3(0.f), glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f,-1.f, 0.f)),
        utils::Transform::makeLookAt(glm::vec3(0.f), glm::vec3( 0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f)),
        utils::Transform::makeLookAt(glm::vec3(0.f), glm::vec3( 0.f,-1.f, 0.f), glm::vec3(0.f, 0.f,-1.f)),
        utils::Transform::makeLookAt(glm::vec3(0.f), glm::vec3( 0.f, 0.f, 1.f), glm::vec3(0.f,-1.f, 0.f)),
        utils::Transform::makeLookAt(glm::vec3(0.f), glm::vec3( 0.f, 0.f,-1.f), glm::vec3(0.f,-1.f, 0.f))
    };

    auto &dPublic = d();

    const auto viewTransform = dPublic.globalTransform().inverted();

    ShadowTransform result;
    result.frustumViewTransform = viewTransform;
    result.frustumClipSpace = utils::ClipSpace::makeSpherical();
    result.layeredViewTransforms.resize(s_layeredLookAt.size());
    for (size_t i = 0; i < s_layeredLookAt.size(); ++i)
        result.layeredViewTransforms[i] = s_layeredLookAt[i] * viewTransform;
    result.clipSpase = utils::ClipSpace::makePerspective(1.f, glm::half_pi<float>());
    result.cullPlanesLimits = m_shadow.cullPlanesLimits() * utils::Range(0.f, dPublic.radiuses()[1u]);
    return result;
}

glm::mat4x4 PointLightNodePrivate::doAreaMatrix()
{
    auto &dPublic = d();
    return glm::scale(glm::mat4x4(1.f), glm::vec3(extendedRadius(dPublic.radiuses()[1u])));
}

utils::BoundingBox PointLightNodePrivate::doAreaBoundingBox()
{
    auto app = ApplicationBase::currentApplication();
    if (!app)
        LOG_CRITICAL << "Application can't be nullptr";

    auto graphicsEngine = app->findEngine<GraphicsEngine>("");
    if (!graphicsEngine)
        LOG_CRITICAL << "Graphics engine can't be nullptr";

    return areaMatrix() * graphicsEngine->m().pointLightAreaBoundingBox();
}

}
}

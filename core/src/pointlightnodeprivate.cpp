#include <utils/logger.h>

#include <core/igraphicswidget.h>
#include <core/graphicsengine.h>
#include <core/graphicsrendererbase.h>
#include <core/pointlightnode.h>
#include <core/scene.h>

#include "graphicsengineprivate.h"
#include "pointlightnodeprivate.h"
#include "scenedata.h"
#include "sceneprivate.h"

namespace simplex
{
namespace core
{

static inline float extendedRadius(float value)
{
    return value * 1.15f;
}

PointLightNodePrivate::PointLightNodePrivate(PointLightNode &pointLightNode, const std::string &name)
    : LightNodePrivate(pointLightNode, name, LightType::Point)
{
}

PointLightNodePrivate::~PointLightNodePrivate() = default;

void PointLightNodePrivate::doAfterTransformChanged()
{
    changeInSceneData();
}

void PointLightNodePrivate::doAttachToScene(const std::shared_ptr<Scene>& scene)
{
    if (auto sceneData = scene->m().sceneData())
        addToSceneData(sceneData);
}

void PointLightNodePrivate::doDetachFromScene(const std::shared_ptr<Scene>&)
{
    removeFromSceneData();
}

glm::vec3& PointLightNodePrivate::color()
{
    return m_color;
}

glm::vec2& PointLightNodePrivate::radiuses()
{
    return m_radiuses;
}

std::shared_ptr<PointLightHandler>& PointLightNodePrivate::handler()
{
    return m_handler;
}

void PointLightNodePrivate::addToSceneData(const std::shared_ptr<SceneData>& sceneData)
{
    if (!sceneData)
    {
        LOG_ERROR << "Scene data can't be nullptr";
        return;
    }

    m_handler = sceneData->addPointLight(d().globalTransform(), doAreaScale(), m_color, m_radiuses);
}

void PointLightNodePrivate::removeFromSceneData()
{
    m_handler.reset();
}

void PointLightNodePrivate::changeInSceneData()
{
    if (m_handler)
        if (auto sceneData = m_handler->sceneData().lock())
            sceneData->onPointLightChanged(m_handler->ID(), d().globalTransform(), doAreaScale(), m_color, m_radiuses);
}

LightNodePrivate::ShadowTransform PointLightNodePrivate::doShadowTransform(const utils::Frustum::Points&)
{
    const std::vector<utils::Transform> layeredLookAt {
        utils::Transform::makeLookAt(glm::vec3(0.f), glm::vec3( 1.f, 0.f, 0.f), glm::vec3(0.f,-1.f, 0.f)),
        utils::Transform::makeLookAt(glm::vec3(0.f), glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f,-1.f, 0.f)),
        utils::Transform::makeLookAt(glm::vec3(0.f), glm::vec3( 0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f)),
        utils::Transform::makeLookAt(glm::vec3(0.f), glm::vec3( 0.f,-1.f, 0.f), glm::vec3(0.f, 0.f,-1.f)),
        utils::Transform::makeLookAt(glm::vec3(0.f), glm::vec3( 0.f, 0.f, 1.f), glm::vec3(0.f,-1.f, 0.f)),
        utils::Transform::makeLookAt(glm::vec3(0.f), glm::vec3( 0.f, 0.f,-1.f), glm::vec3(0.f,-1.f, 0.f))
    };

    const auto numLayers = numLayeredShadowMatrices(LightType::Point);
    assert(layeredLookAt.size() == numLayers);

    auto &dPublic = d();

    const auto viewTransform = dPublic.globalTransform().inverted();

    ShadowTransform result;
    result.frustumViewTransform = viewTransform;
    result.frustumClipSpace = utils::ClipSpace::makeSpherical();
    result.layeredViewTransforms.resize(numLayers);
    for (uint32_t i = 0; i < numLayers; ++i)
        result.layeredViewTransforms[i] = layeredLookAt[i] * viewTransform;
    result.clipSpase = utils::ClipSpace::makePerspective(1.f, glm::half_pi<float>());
    result.cullPlanesLimits = shadow().cullPlanesLimits() * utils::Range(0.f, dPublic.radiuses()[1u]);
    return result;
}

glm::mat4x4 PointLightNodePrivate::doAreaMatrix()
{
    auto &dPublic = d();
    return glm::scale(glm::mat4x4(1.f), glm::vec3(extendedRadius(dPublic.radiuses()[1u])));
}

utils::BoundingBox PointLightNodePrivate::doAreaBoundingBox()
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

    return areaMatrix() * graphicsEngine->m().pointLightAreaBoundingBox();
}

glm::vec3 PointLightNodePrivate::doAreaScale() const
{
    return glm::vec3(extendedRadius(d().radiuses()[1u]));
}

}
}

#include <core/directionallightnode.h>
#include <core/scene.h>

#include "directionallightnodeprivate.h"
#include "sceneprivate.h"
#include "scenedata.h"

namespace simplex
{
namespace core
{

DirectionalLightNodePrivate::DirectionalLightNodePrivate(DirectionalLightNode &directionalLightNode, const std::string &name)
    : LightNodePrivate(directionalLightNode, name, LightType::Directional)
{
}

DirectionalLightNodePrivate::~DirectionalLightNodePrivate() = default;

void DirectionalLightNodePrivate::doAfterTransformChanged()
{
    changeInSceneData();
}

void DirectionalLightNodePrivate::doAttachToScene(const std::shared_ptr<Scene>& scene)
{
    if (auto& sceneData = scene->m().sceneData())
        addToSceneData(sceneData);
}

void DirectionalLightNodePrivate::doDetachFromScene(const std::shared_ptr<Scene>& scene)
{
    removeFromSceneData();
}

glm::vec3& DirectionalLightNodePrivate::color()
{
    return m_color;
}

void DirectionalLightNodePrivate::addToSceneData(const std::shared_ptr<SceneData>&sceneData)
{
    if (!sceneData)
    {
        LOG_ERROR << "Scene data can't be nullptr";
        return;
    }

    m_handler = sceneData->addDirectionalLight(d().globalTransform(), m_color);
}

void DirectionalLightNodePrivate::removeFromSceneData()
{
    m_handler.reset();
}

void DirectionalLightNodePrivate::changeInSceneData()
{
    if (m_handler)
        if (auto sceneData = m_handler->sceneData().lock())
            sceneData->onDirectionalLightChanged(m_handler->ID(), d().globalTransform(), m_color);
}

std::shared_ptr<LightHandler>& DirectionalLightNodePrivate::handler()
{
    return m_handler;
}

LightNodePrivate::ShadowTransform DirectionalLightNodePrivate::doShadowTransform(const utils::Frustum::Points &cameraFrustumPoints)
{
    static const auto s_directionInLightSpace = glm::vec4(0.f, 0.f, -1.f, 0.f);
    const auto direction = glm::normalize(glm::vec3(static_cast<glm::mat4x4>(d_.globalTransform()) * s_directionInLightSpace));
    const auto up = glm::abs(direction.y) < (1.f - utils::epsilon<float>()) ? glm::vec3(0.f, 1.f, 0.f) : glm::vec3(1.f, 0.f, 0.f);

    utils::BoundingBox cameraFrustumBB;
    for (const auto &[index, corner] : cameraFrustumPoints)
        cameraFrustumBB += corner;

    const auto sceneRootNode = d_.rootNode();
    const auto sceneBB = sceneRootNode->globalTransform() * utils::BoundingBox();// sceneRootNode->boundingBox();

    const auto shadowBB = cameraFrustumBB * sceneBB;
    const auto shadowBBCenter = shadowBB.center();

    const auto sceneBBProjRange = sceneBB.projectOnLine(utils::Line(shadowBBCenter, direction));

    const auto viewTransform = utils::Transform(1.f,
                                                glm::quatLookAt(direction, up),
                                                shadowBBCenter + direction * sceneBBProjRange.nearValue()).inverted();

    const auto transformedShadowBBHalfSize = (viewTransform * shadowBB).halfSize();
    const auto clipSpace = utils::ClipSpace::makeOrtho(-transformedShadowBBHalfSize.x, transformedShadowBBHalfSize.x,
                                                       -transformedShadowBBHalfSize.y, transformedShadowBBHalfSize.y);

    const auto numLayers = numLayeredShadowMatrices();

    ShadowTransform result;
    result.frustumViewTransform = viewTransform;
    result.frustumClipSpace = clipSpace;
    result.layeredViewTransforms.resize(numLayers);
    for (uint32_t i = 0; i < numLayers; ++i)
        result.layeredViewTransforms[i] = viewTransform;
    result.clipSpase = clipSpace;
    result.cullPlanesLimits = shadow().cullPlanesLimits();
    return result;
}

}
}

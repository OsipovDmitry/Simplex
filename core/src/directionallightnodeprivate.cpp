#include <core/directionallightnode.h>

#include "directionallightnodeprivate.h"
#include "directionalshadowprivate.h"

namespace simplex
{
namespace core
{

DirectionalLightNodePrivate::DirectionalLightNodePrivate(DirectionalLightNode &directionalLightNode, const std::string &name)
    : LightNodePrivate(directionalLightNode, name, std::make_unique<DirectionalShadowPrivate>())
{
}

void DirectionalLightNodePrivate::doAfterTransformChanged()
{
    dirtyAreaMatrix();
    dirtyAreaBoundingBox();
}

DirectionalLightNodePrivate::~DirectionalLightNodePrivate() = default;

LightNodePrivate::ShadowTransform DirectionalLightNodePrivate::doShadowTransform(const utils::Frustum::Points &cameraFrustumPoints)
{
    static const auto s_directionInLightSpace = glm::vec3(0.f, 0.f, -1.f);
    const auto direction = glm::normalize(glm::vec3(d_.globalTransform() * glm::vec4(s_directionInLightSpace, 0.f)));
    const auto up = glm::abs(direction.y) < (1.f - utils::epsilon<float>()) ? glm::vec3(0.f, 1.f, 0.f) : glm::vec3(1.f, 0.f, 0.f);

    utils::BoundingBox cameraFrustumBB;
    for (const auto &[index, corner] : cameraFrustumPoints)
        cameraFrustumBB += corner;

    const auto sceneRootNode = d_.rootNode();
    const auto sceneBB = sceneRootNode->globalTransform() * sceneRootNode->boundingBox();

    const auto shadowBB = cameraFrustumBB * sceneBB;
    const auto shadowBBCenter = shadowBB.center();

    const auto sceneBBProjRange = sceneBB.projectOnLine(utils::Line(shadowBBCenter, direction));

    const auto viewTransform = utils::Transform(1.f,
                                                glm::quatLookAt(direction, up),
                                                shadowBBCenter + direction * sceneBBProjRange.nearValue()).inverted();

    const auto transformedShadowBBHalfSize = (viewTransform * shadowBB).halfSize();
    const auto clipSpace = utils::ClipSpace::makeOrtho(-transformedShadowBBHalfSize.x, transformedShadowBBHalfSize.x,
                                                       -transformedShadowBBHalfSize.y, transformedShadowBBHalfSize.y);

    ShadowTransform result;
    result.frustumViewTransform = viewTransform;
    result.frustumClipSpace = clipSpace;
    result.layeredViewTransforms = { viewTransform };
    result.clipSpase = clipSpace;
    result.cullPlanesLimits = m_shadow.cullPlanesLimits();
    return result;
}

glm::mat4x4 DirectionalLightNodePrivate::doAreaMatrix()
{
    return glm::mat4x4(1.f); // it is not used because bb policy is Root
}

utils::BoundingBox DirectionalLightNodePrivate::doAreaBoundingBox()
{
    return utils::BoundingBox::empty(); // it is not used because bb policy is Root
}

}
}

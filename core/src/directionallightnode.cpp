#include <utils/logger.h>
#include <utils/frustum.h>

#include <core/igraphicsrenderer.h>
#include <core/uniform.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/directionallightnode.h>
#include <core/lightdrawable.h>

#include "directionallightnodeprivate.h"

namespace simplex
{
namespace core
{

DirectionalLightNode::DirectionalLightNode(const std::string &name)
    : LightNode(std::make_unique<DirectionalLightNodePrivate>(name))
{
    if (DirectionalLightNodePrivate::lightAreaVertexArray().expired())
        LOG_CRITICAL << "Directional light area vertex array is expired";

    auto drawable = std::make_shared<LightDrawable>(DirectionalLightNodePrivate::lightAreaVertexArray().lock());
    m().areaDrawable() = drawable;
    drawable->getOrCreateUniform(graphics::UniformId::LightColor) = makeUniform(glm::vec3(1.f));
}

LightType DirectionalLightNode::type() const
{
    return LightType::Directional;
}

DirectionalLightNode::~DirectionalLightNode() = default;

std::shared_ptr<DirectionalLightNode> DirectionalLightNode::asDirectionalLightNode()
{
    return std::dynamic_pointer_cast<DirectionalLightNode>(shared_from_this());
}

std::shared_ptr<const DirectionalLightNode> DirectionalLightNode::asDirectionalLightNode() const
{
    return std::dynamic_pointer_cast<const DirectionalLightNode>(shared_from_this());
}

const glm::vec3 &DirectionalLightNode::color() const
{
    return uniform_cast<glm::vec3>(m().areaDrawable()->uniform(graphics::UniformId::LightColor))->data();
}

void DirectionalLightNode::setColor(const glm::vec3 &value)
{
    uniform_cast<glm::vec3>(m().areaDrawable()->uniform(graphics::UniformId::LightColor))->data() = value;
}

void DirectionalLightNode::doAfterTransformChanged()
{
    recalculateAreaBoundingBox();
    m().isAreaMatrixDirty() = true;
}

glm::mat4x4 DirectionalLightNode::doAreaMatrix() const
{
    return glm::mat4x4(1.f); // it is not used because bb policy is Root
}

utils::BoundingBox DirectionalLightNode::doAreaBoundingBox() const
{
    return utils::BoundingBox::empty(); // it is not used because bb policy is Root
}

glm::mat4x4 DirectionalLightNode::doUpdateLayeredShadowMatrices(const utils::FrustumCorners &cameraFrustumCorners,
                                                                const utils::Range &zRange,
                                                                std::vector<glm::mat4x4> &layeredShadowMatrices) const
{
    static const auto s_directionInLightSpace = glm::vec3(0.f, 0.f, -1.f);
    const auto direction = glm::normalize(glm::vec3(globalTransform() * glm::vec4(s_directionInLightSpace, 0.f)));
    const auto up = glm::abs(direction.y) < .999f ? glm::vec3(0.f, 1.f, 0.f) : glm::vec3(1.f, 0.f, 0.f);

    utils::BoundingBox cameraFrustumBB;
    for (const auto &corner : cameraFrustumCorners)
        cameraFrustumBB += corner;

    const auto sceneRootNode = rootNode();
    const auto sceneBB = sceneRootNode->globalTransform() * sceneRootNode->boundingBox();

    const auto shadowBB = cameraFrustumBB * sceneBB;
    const auto shadowBBCenter = shadowBB.center();

    const auto sceneBBProjRange = sceneBB.projectOnLine(shadowBBCenter, direction);

    const auto viewTransform = utils::Transform(1.f,
                                                glm::quatLookAt(direction, up),
                                                shadowBBCenter + direction * sceneBBProjRange.nearValue()).inverted();

    const auto transformedShadowBBHalfSize = (viewTransform * shadowBB).halfSize();
    const auto projectionMatrix = glm::ortho(-transformedShadowBBHalfSize.x, transformedShadowBBHalfSize.x,
                                             -transformedShadowBBHalfSize.y, transformedShadowBBHalfSize.y,
                                             zRange.nearValue(), zRange.farValue());

    const auto result = projectionMatrix * viewTransform;
    layeredShadowMatrices = { result };
    return result;
}

}
}

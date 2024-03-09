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

    auto &mPrivate = m();
    mPrivate.boundingBoxPolicy() = BoundingBoxPolicy::Root;

    auto drawable = std::make_shared<LightDrawable>(DirectionalLightNodePrivate::lightAreaVertexArray().lock(), LightDrawableType::Directional);
    mPrivate.areaDrawable() = drawable;
    drawable->getOrCreateUniform(graphics::UniformId::LightColor) = makeUniform(glm::vec3(1.f));
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

utils::Transform DirectionalLightNode::doShadowViewTransform(const utils::FrustumCornersInfo &cameraFrustumCornersInfo) const
{
    const auto lightDirection = glm::normalize(glm::vec3(globalTransform() * glm::vec4(0.f, 0.f, -1.f, 0.f)));
    auto bbRange = (globalTransform() * boundingBox()).projectOnLine(cameraFrustumCornersInfo.center, lightDirection);
    bbRange.setNearValue(bbRange.nearValue() /*- 1.f*/);

    const auto up = glm::abs(lightDirection.y) < .999f ? glm::vec3(0.f, 1.f, 0.f) : glm::vec3(1.f, 0.f, 0.f);
    return utils::Transform(1.f,
                            glm::quatLookAt(lightDirection, up),
                            cameraFrustumCornersInfo.center + bbRange.nearValue() * lightDirection).inverted();
}

glm::mat4x4 DirectionalLightNode::doShadowProjectionMatrix(const utils::Transform &shadowViewTransform,
                                                           const utils::FrustumCornersInfo &cameraFrustumCornersInfo,
                                                           const utils::Range &zRange) const
{
    utils::BoundingBox lightFrustumBB;
    for (const auto &corner : cameraFrustumCornersInfo.corners)
        lightFrustumBB += shadowViewTransform * corner;

    return glm::ortho(lightFrustumBB.minPoint().x, lightFrustumBB.maxPoint().x,
                      lightFrustumBB.minPoint().y, lightFrustumBB.maxPoint().y,
                      /*1.f, -lightFrustumBB.minPoint().z*/
                      zRange.nearValue(), zRange.farValue());
}

}
}

#include <utils/logger.h>
#include <utils/frustum.h>

#include <core/igraphicsrenderer.h>
#include <core/uniform.h>
#include <core/scene.h>
#include <core/spotlightnode.h>
#include <core/lightdrawable.h>

#include "spotlightnodeprivate.h"

namespace simplex
{
namespace core
{

inline float extendedRadius(float value)
{
    return value * 1.15f;
}

SpotLightNode::SpotLightNode(const std::string &name)
    : LightNode(std::make_unique<SpotLightNodePrivate>(name))
{
    static const auto s_defaultHalfAngles = glm::quarter_pi<float>() * glm::vec2(0.f, 1.f);

    if (SpotLightNodePrivate::lightAreaVertexArray().expired())
        LOG_CRITICAL << "Spot light area vertex array is expired";

    auto &mPrivate = m();
    mPrivate.halfAngles() = s_defaultHalfAngles;

    auto drawable = std::make_shared<LightDrawable>(SpotLightNodePrivate::lightAreaVertexArray().lock());
    mPrivate.areaDrawable() = drawable;
    drawable->getOrCreateUniform(graphics::UniformId::LightColor) = makeUniform(glm::vec3(1.f));
    drawable->getOrCreateUniform(graphics::UniformId::LightRadiuses) = makeUniform(glm::vec2(1.f, 2.f));
    drawable->getOrCreateUniform(graphics::UniformId::LightCosHalfAngles) = makeUniform(glm::cos(s_defaultHalfAngles));
}

LightType SpotLightNode::type() const
{
    return LightType::Spot;
}

SpotLightNode::~SpotLightNode() = default;

std::shared_ptr<SpotLightNode> SpotLightNode::asSpotLightNode()
{
    return std::dynamic_pointer_cast<SpotLightNode>(shared_from_this());
}

std::shared_ptr<const SpotLightNode> SpotLightNode::asSpotLightNode() const
{
    return std::dynamic_pointer_cast<const SpotLightNode>(shared_from_this());
}

const glm::vec3 &SpotLightNode::color() const
{
    return uniform_cast<glm::vec3>(m().areaDrawable()->uniform(graphics::UniformId::LightColor))->data();
}

void SpotLightNode::setColor(const glm::vec3 &value)
{
    uniform_cast<glm::vec3>(m().areaDrawable()->uniform(graphics::UniformId::LightColor))->data() = value;
}

const glm::vec2 &SpotLightNode::radiuses() const
{
    return uniform_cast<glm::vec2>(m().areaDrawable()->uniform(graphics::UniformId::LightRadiuses))->data();
}

void SpotLightNode::setRadiuses(const glm::vec2 &value)
{
    if (value[0] < 0.f)
        LOG_CRITICAL << "minRadius must be greater or equal than 0.0";

    if (value[1] <= value[0])
        LOG_CRITICAL << "maxRadius must be greater than minRadius";

    auto &mPrivate = m();
    uniform_cast<glm::vec2>(mPrivate.areaDrawable()->uniform(graphics::UniformId::LightRadiuses))->data() = value;

    recalculateAreaBoundingBox();
    mPrivate.isAreaMatrixDirty() = true;
}

const glm::vec2 &SpotLightNode::halfAngles() const
{
    return m().halfAngles();
}

void SpotLightNode::setHalfAngles(const glm::vec2 &value)
{
    if (value[0] < 0.f)
        LOG_CRITICAL << "Min angle must be greater or equal than 0.0";

    if (value[0] >= value[1])
        LOG_CRITICAL << "Max angle must be greater than min angle";

    if (value[1] >= glm::half_pi<float>())
        LOG_CRITICAL << "Max angle must be less than pi/2";

    auto &mPrivate = m();
    mPrivate.halfAngles() = value;
    uniform_cast<glm::vec2>(mPrivate.areaDrawable()->uniform(graphics::UniformId::LightCosHalfAngles))->data() = glm::cos(value);

    recalculateAreaBoundingBox();
    mPrivate.isAreaMatrixDirty() = true;
}

glm::mat4x4 SpotLightNode::doAreaMatrix() const
{
    return glm::scale(glm::mat4x4(1.f), glm::vec3(extendedRadius(radiuses()[1u])) * glm::vec3(glm::vec2(glm::tan(halfAngles()[1u])), 1.f));
}

utils::BoundingBox SpotLightNode::doAreaBoundingBox() const
{
    return areaMatrix() * SpotLightNodePrivate::lightAreaBoundingBox();
}

glm::mat4x4 SpotLightNode::doUpdateLayeredShadowMatrices(const utils::FrustumCorners &cameraFrustumCorners,
                                                         const utils::Range &zRange,
                                                         std::vector<glm::mat4x4> &layeredShadowMatrices) const
{
    const auto viewTransform = globalTransform().inverted();

    utils::BoundingBoxT<2u, float> cameraFrustumBB;
    for (const auto &corner : cameraFrustumCorners)
    {
        auto cornerInShadowSpace = viewTransform * corner;
        cornerInShadowSpace.z = glm::min(cornerInShadowSpace.z, -zRange.nearValue());
        cameraFrustumBB += -cornerInShadowSpace * zRange.nearValue() / cornerInShadowSpace.z;
    }

    const float t = zRange.nearValue() * glm::atan(halfAngles()[1u]);
    const utils::BoundingBoxT<2u, float> lightFrustumBB(glm::vec2(-t, -t), glm::vec2(t, t));

    const auto shadowBB = cameraFrustumBB * lightFrustumBB;

    const auto projectionMatrix = glm::frustum(shadowBB.minPoint().x, shadowBB.maxPoint().x,
                                               shadowBB.minPoint().y, shadowBB.maxPoint().y,
                                               zRange.nearValue(), zRange.farValue());

    const auto result = projectionMatrix * viewTransform;
    layeredShadowMatrices = { result };
    return result;
}

}
}

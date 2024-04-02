#include <utils/logger.h>
#include <utils/frustum.h>

#include <core/igraphicsrenderer.h>
#include <core/uniform.h>
#include <core/scene.h>
#include <core/pointlightnode.h>
#include <core/lightdrawable.h>

#include "pointlightnodeprivate.h"

namespace simplex
{
namespace core
{

inline float extendedRadius(float value)
{
    return value * 1.15f;
}

PointLightNode::PointLightNode(const std::string &name)
    : LightNode(std::make_unique<PointLightNodePrivate>(name))
{
    if (PointLightNodePrivate::lightAreaVertexArray().expired())
        LOG_CRITICAL << "Point light area vertex array is expired";

    auto drawable = std::make_shared<LightDrawable>(PointLightNodePrivate::lightAreaVertexArray().lock());
    drawable->getOrCreateUniform(graphics::UniformId::LightColor) = makeUniform(glm::vec3(1.f));
    drawable->getOrCreateUniform(graphics::UniformId::LightRadiuses) = makeUniform(glm::vec2(1.f, 2.f));
    m().areaDrawable() = drawable;
}

LightType PointLightNode::type() const
{
    return LightType::Point;
}

PointLightNode::~PointLightNode() = default;

std::shared_ptr<PointLightNode> PointLightNode::asPointLightNode()
{
    return std::dynamic_pointer_cast<PointLightNode>(shared_from_this());
}

std::shared_ptr<const PointLightNode> PointLightNode::asPointLightNode() const
{
    return std::dynamic_pointer_cast<const PointLightNode>(shared_from_this());
}

const glm::vec3 &PointLightNode::color() const
{
    return uniform_cast<glm::vec3>(m().areaDrawable()->uniform(graphics::UniformId::LightColor))->data();
}

void PointLightNode::setColor(const glm::vec3 &value)
{
    uniform_cast<glm::vec3>(m().areaDrawable()->uniform(graphics::UniformId::LightColor))->data() = value;
}

const glm::vec2 &PointLightNode::radiuses() const
{
    return uniform_cast<glm::vec2>(m().areaDrawable()->uniform(graphics::UniformId::LightRadiuses))->data();
}

void PointLightNode::setRadiuses(const glm::vec2 &value)
{
    if (value[0u] < 0.f)
        LOG_CRITICAL << "minRadius must be greater or equal than 0.0";

    if (value[1u] <= value[0])
        LOG_CRITICAL << "maxRadius must be greater than minRadius";

    auto &mPrivate = m();
    uniform_cast<glm::vec2>(mPrivate.areaDrawable()->uniform(graphics::UniformId::LightRadiuses))->data() = value;

    recalculateAreaBoundingBox();
    mPrivate.isAreaMatrixDirty() = true;
}

glm::mat4x4 PointLightNode::doAreaMatrix() const
{
    return glm::scale(glm::mat4x4(1.f), glm::vec3(extendedRadius(radiuses()[1u])));
}

utils::BoundingBox PointLightNode::doAreaBoundingBox() const
{
    return areaMatrix() * PointLightNodePrivate::lightAreaBoundingBox();
}

glm::mat4x4 PointLightNode::doUpdateLayeredShadowMatrices(const utils::FrustumCorners &cameraFrustumCorners,
                                                          const utils::Range &zRange,
                                                          std::vector<glm::mat4x4> &layeredShadowMatrices) const
{
    static const std::array<glm::mat4x4, 6u> layeredLookAt {
        glm::lookAt(glm::vec3(0.f), glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
        glm::lookAt(glm::vec3(0.f), glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f,-1.f, 0.f)),
        glm::lookAt(glm::vec3(0.f), glm::vec3( 0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f)),
        glm::lookAt(glm::vec3(0.f), glm::vec3( 0.f,-1.f, 0.f), glm::vec3(0.f, 0.f,-1.f)),
        glm::lookAt(glm::vec3(0.f), glm::vec3( 0.f, 0.f, 1.f), glm::vec3(0.f,-1.f, 0.f)),
        glm::lookAt(glm::vec3(0.f), glm::vec3( 0.f, 0.f,-1.f), glm::vec3(0.f,-1.f, 0.f))
    };

    const auto viewTransform = globalTransform().inverted();
    const auto layeredProjectionMatrix = glm::perspective(glm::half_pi<float>(), 1.f, zRange.nearValue(), zRange.farValue());

    layeredShadowMatrices.resize(layeredLookAt.size());
    for (uint32_t i = 0u; i < layeredShadowMatrices.size(); ++i)
        layeredShadowMatrices[i] = layeredProjectionMatrix * layeredLookAt[i] * viewTransform;

    return viewTransform;
}

}
}

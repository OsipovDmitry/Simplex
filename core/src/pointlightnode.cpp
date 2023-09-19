#include <utils/logger.h>

#include <core/igraphicsrenderer.h>
#include <core/uniform.h>
#include <core/scene.h>
#include <core/pointlightnode.h>
#include <core/drawablebase.h>

#include "pointlightnodeprivate.h"

namespace simplex
{
namespace core
{

PointLightNode::PointLightNode(const std::string &name)
    : LightNode(std::make_unique<PointLightNodePrivate>(name))
{
    if (PointLightNodePrivate::lightAreaVertexArray().expired())
        LOG_CRITICAL << "Point light area vertex array is expired";

    auto drawable = std::make_shared<DrawableBase>(PointLightNodePrivate::lightAreaVertexArray().lock());
    drawable->getOrCreateUniform(graphics::UniformId::LightColor) = makeUniform(glm::vec3(1.f));
    drawable->getOrCreateUniform(graphics::UniformId::LightRadiuses) = makeUniform(glm::vec2(1.f, 2.f));
    m().areaDrawable() = drawable;
}

PointLightNode::~PointLightNode() = default;

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
    if (value[0] < 0.f)
        LOG_CRITICAL << "minRadius must be greater or equal than 0.0";

    if (value[1] <= value[0])
        LOG_CRITICAL << "maxRadius must be greater than minRadius";

    auto &mPrivate = m();
    uniform_cast<glm::vec2>(mPrivate.areaDrawable()->uniform(graphics::UniformId::LightRadiuses))->data() = value;
    mPrivate.isAreaMatrixDirty() = true;
}

glm::mat4x4 PointLightNode::doAreaMatrix() const
{
    return glm::scale(glm::mat4x4(1.f), glm::vec3(radiuses()[1]));
}

}
}

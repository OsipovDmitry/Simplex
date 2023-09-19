#include <utils/logger.h>

#include <core/igraphicsrenderer.h>
#include <core/uniform.h>
#include <core/scene.h>
#include <core/spotlightnode.h>
#include <core/drawable.h>

#include "spotlightnodeprivate.h"

namespace simplex
{
namespace core
{

SpotLightNode::SpotLightNode(const std::string &name)
    : LightNode(std::make_unique<SpotLightNodePrivate>(name))
{
    if (SpotLightNodePrivate::lightAreaVertexArray().expired())
        LOG_CRITICAL << "Point light area vertex array is expired";

    auto drawable = std::make_shared<Drawable>(SpotLightNodePrivate::lightAreaVertexArray().lock());
    drawable->getOrCreateUniform(graphics::UniformId::LightColor) = makeUniform(glm::vec3(1.f));
    drawable->getOrCreateUniform(graphics::UniformId::LightRadiuses) = makeUniform(glm::vec2(1.f, 2.f));
    drawable->getOrCreateUniform(graphics::UniformId::LightHalfAngles) = makeUniform(glm::vec2(.5f * glm::quarter_pi<float>(),
                                                                                               glm::quarter_pi<float>()));
    m().areaDrawable() = drawable;
}

SpotLightNode::~SpotLightNode() = default;

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
    uniform_cast<glm::vec2>(m().areaDrawable()->uniform(graphics::UniformId::LightRadiuses))->data() = value;
    mPrivate.isAreaMatrixDirty() = true;
}

const glm::vec2 &SpotLightNode::halfAngles() const
{
    return uniform_cast<glm::vec2>(m().areaDrawable()->uniform(graphics::UniformId::LightHalfAngles))->data();
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
    uniform_cast<glm::vec2>(m().areaDrawable()->uniform(graphics::UniformId::LightHalfAngles))->data() = value;
    mPrivate.isAreaMatrixDirty() = true;
}

glm::mat4x4 SpotLightNode::doAreaMatrix() const
{
    return glm::scale(glm::mat4x4(1.f), glm::vec3(radiuses()[1]) * glm::vec3(glm::vec2(glm::tan(halfAngles()[1])), 1.f));
}

}
}

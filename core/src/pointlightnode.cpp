#include <utils/logger.h>
#include <utils/clipspace.h>

#include <core/applicationbase.h>
#include <core/graphicsengine.h>
#include <core/scene.h>
#include <core/pointlightnode.h>
#include <core/uniform.h>

#include "graphicsengineprivate.h"
#include "pointlightnodeprivate.h"
#include "lightdrawable.h"

namespace simplex
{
namespace core
{

PointLightNode::PointLightNode(const std::string &name)
    : LightNode(std::make_unique<PointLightNodePrivate>(*this, name))
{
    auto app = ApplicationBase::currentApplication();
    if (!app)
        LOG_CRITICAL << "Application can't be nullptr";

    auto graphicsEngine = app->findEngine<GraphicsEngine>("");
    if (!graphicsEngine)
        LOG_CRITICAL << "Graphics engine can't be nullptr";

    auto &mPrivate = m();
    mPrivate.areaDrawable() = std::make_shared<LightDrawable>(graphicsEngine->m().pointLightAreaVertexArray());

    setColor(glm::vec3(1.f));
    setRadiuses(glm::vec2(1.f, 2.f));
}

LightType PointLightNode::type() const
{
    return LightType::Point;
}

PointLightNode::~PointLightNode() = default;

std::shared_ptr<PointLightNode> PointLightNode::asPointLightNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<PointLightNode>(wp.lock());
}

std::shared_ptr<const PointLightNode> PointLightNode::asPointLightNode() const
{
    return const_cast<PointLightNode*>(this)->asPointLightNode();
}

const glm::vec3 &PointLightNode::color() const
{
    return uniform_cast<glm::vec3>(m().areaDrawable()->uniform(UniformId::LightColor))->data();
}

void PointLightNode::setColor(const glm::vec3 &value)
{
    m().areaDrawable()->getOrCreateUniform(UniformId::LightColor) = makeUniform(value);
}

const glm::vec2 &PointLightNode::radiuses() const
{
    return uniform_cast<glm::vec2>(m().areaDrawable()->uniform(UniformId::LightRadiuses))->data();
}

void PointLightNode::setRadiuses(const glm::vec2 &value)
{
    if (value[0u] < 0.f)
        LOG_CRITICAL << "minRadius must be greater or equal than 0.0";

    if (value[1u] <= value[0])
        LOG_CRITICAL << "maxRadius must be greater than minRadius";

    auto &mPrivate = m();
    mPrivate.areaDrawable()->getOrCreateUniform(UniformId::LightRadiuses) = makeUniform(value);

    mPrivate.dirtyAreaMatrix();
    mPrivate.dirtyAreaBoundingBox();
}

}
}

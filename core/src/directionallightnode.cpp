#include <utils/logger.h>
#include <utils/clipspace.h>

#include <core/igraphicswidget.h>
#include <core/graphicsengine.h>
#include <core/uniform.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/directionallightnode.h>

#include "graphicsengineprivate.h"
#include "directionallightnodeprivate.h"
#include "lightdrawable.h"

namespace simplex
{
namespace core
{

DirectionalLightNode::DirectionalLightNode(const std::string &name)
    : LightNode(std::make_unique<DirectionalLightNodePrivate>(*this, name))
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

    auto &mPrivate = m();
    mPrivate.areaDrawable() = std::make_shared<LightDrawable>(graphicsEngine->m().directionalLightAreaVertexArray());

    setColor(glm::vec3(1.f));
}

LightType DirectionalLightNode::type() const
{
    return LightType::Directional;
}

DirectionalLightNode::~DirectionalLightNode() = default;

std::shared_ptr<DirectionalLightNode> DirectionalLightNode::asDirectionalLightNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<DirectionalLightNode>(wp.lock());
}

std::shared_ptr<const DirectionalLightNode> DirectionalLightNode::asDirectionalLightNode() const
{
    return const_cast<DirectionalLightNode*>(this)->asDirectionalLightNode();
}

const glm::vec3 &DirectionalLightNode::color() const
{
    return uniform_cast<glm::vec3>(m().areaDrawable()->uniform(UniformId::LightColor))->data();
}

void DirectionalLightNode::setColor(const glm::vec3 &value)
{
    m().areaDrawable()->getOrCreateUniform(UniformId::LightColor) = makeUniform(value);
}

}
}

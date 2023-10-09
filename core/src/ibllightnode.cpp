#include <utils/logger.h>

#include <core/igraphicsrenderer.h>
#include <core/uniform.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/ibllightnode.h>
#include <core/lightdrawable.h>

#include "ibllightnodeprivate.h"

namespace simplex
{
namespace core
{

IBLLightNode::IBLLightNode(const std::string &name)
    : LightNode(std::make_unique<IBLLightNodePrivate>(name))
{
    if (IBLLightNodePrivate::lightAreaVertexArray().expired())
        LOG_CRITICAL << "IBL light area vertex array is expired";

    if (IBLLightNodePrivate::defaultDiffuseTexture().expired())
        LOG_CRITICAL << "IBL default diffuse texture is expired";

    if (IBLLightNodePrivate::defaultSpecularTexture().expired())
        LOG_CRITICAL << "IBL default specular texture is expired";

    auto drawable = std::make_shared<LightDrawable>(IBLLightNodePrivate::lightAreaVertexArray().lock(), LightDrawableType::IBL);
    drawable->getOrCreateUniform(graphics::UniformId::IBLDiffuseMap) = makeUniform(IBLLightNodePrivate::defaultDiffuseTexture().lock());
    drawable->getOrCreateUniform(graphics::UniformId::IBLSpecularMap) = makeUniform(IBLLightNodePrivate::defaultSpecularTexture().lock());
    m().areaDrawable() = drawable;
}

IBLLightNode::~IBLLightNode() = default;

std::shared_ptr<IBLLightNode> IBLLightNode::asIBLLightNode()
{
    return std::dynamic_pointer_cast<IBLLightNode>(shared_from_this());
}

std::shared_ptr<const IBLLightNode> IBLLightNode::asIBLLightNode() const
{
    return std::dynamic_pointer_cast<const IBLLightNode>(shared_from_this());
}

const graphics::PConstTexture &IBLLightNode::duffuseTexture() const
{
    return uniform_cast<graphics::PConstTexture>(m().areaDrawable()->uniform(graphics::UniformId::IBLDiffuseMap))->data();
}

void IBLLightNode::setDuffuseTexture(const graphics::PConstTexture &value)
{
    uniform_cast<graphics::PConstTexture>(m().areaDrawable()->uniform(graphics::UniformId::IBLDiffuseMap))->data() = value;
}

const graphics::PConstTexture &IBLLightNode::specularTexture() const
{
    return uniform_cast<graphics::PConstTexture>(m().areaDrawable()->uniform(graphics::UniformId::IBLSpecularMap))->data();
}

void IBLLightNode::setSpecularTexture(const graphics::PConstTexture &value)
{
    uniform_cast<graphics::PConstTexture>(m().areaDrawable()->uniform(graphics::UniformId::IBLSpecularMap))->data() = value;
}

void IBLLightNode::doAfterTransformChanged()
{
    m().isAreaMatrixDirty() = true;
}

glm::mat4x4 IBLLightNode::doAreaMatrix() const
{
    glm::mat4x4 result(1.f);

    std::shared_ptr<const Node> sceneRootNode;
    if (auto s = scene(); s)
        sceneRootNode = s->sceneRootNode();

    if (!sceneRootNode)
        return result;

    auto sceneBoundingBox = sceneRootNode->globalTransform() * sceneRootNode->boundingBox();
    result = globalTransform().inverted() *
            glm::translate(glm::mat4x4(1.f), sceneBoundingBox.center()) *
            glm::scale(glm::mat4x4(1.f), sceneBoundingBox.halfSize());

    return result;
}

}
}

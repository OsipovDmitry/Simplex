#include <utils/logger.h>

#include <core/igraphicsrenderer.h>
#include <core/uniform.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/directionallightnode.h>
#include <core/drawable.h>

#include "directionallightnodeprivate.h"

namespace simplex
{
namespace core
{

DirectionalLightNode::DirectionalLightNode(const std::string &name)
    : LightNode(std::make_unique<DirectionalLightNodePrivate>(name))
{
    if (DirectionalLightNodePrivate::lightAreaVertexArray().expired())
        LOG_CRITICAL << "Point light area vertex array is expired";

    auto drawable = std::make_shared<Drawable>(DirectionalLightNodePrivate::lightAreaVertexArray().lock());
    drawable->getOrCreateUniform(graphics::UniformId::LightColor) = makeUniform(glm::vec3(1.f));
    m().areaDrawable() = drawable;
}

DirectionalLightNode::~DirectionalLightNode() = default;

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
    m().isAreaMatrixDirty() = true;
}

glm::mat4x4 DirectionalLightNode::doAreaMatrix() const
{
    glm::mat4x4 result(1.f);

    std::shared_ptr<const Node> sceneRootNode;
    if (auto s = scene(); s)
        sceneRootNode = s->sceneRootNode();

    if (!sceneRootNode)
        return result;

    auto sceneBoundingBox = sceneRootNode->globalTransform() * sceneRootNode->boundingBox();
    result = globalTransform().inverted() * glm::scale(glm::mat4x4(1.f), sceneBoundingBox.halfSize());

    return result;
}

}
}

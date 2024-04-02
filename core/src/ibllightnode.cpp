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
    setShadingMode(LightShadingMode::Disabled);

    if (IBLLightNodePrivate::lightAreaVertexArray().expired())
        LOG_CRITICAL << "IBL light area vertex array is expired";

    if (IBLLightNodePrivate::defaultBRDFLutMap().expired())
        LOG_CRITICAL << "IBL default BRDF Lut texture is expired";

    if (IBLLightNodePrivate::defaultDiffuseMap().expired())
        LOG_CRITICAL << "IBL default diffuse texture is expired";

    if (IBLLightNodePrivate::defaultSpecularMap().expired())
        LOG_CRITICAL << "IBL default specular texture is expired";

    auto drawable = std::make_shared<LightDrawable>(IBLLightNodePrivate::lightAreaVertexArray().lock());
    m().areaDrawable() = drawable;
    drawable->getOrCreateUniform(graphics::UniformId::IBLBRDFLutMap) = makeUniform(IBLLightNodePrivate::defaultBRDFLutMap().lock());
    drawable->getOrCreateUniform(graphics::UniformId::IBLDiffuseMap) = makeUniform(IBLLightNodePrivate::defaultDiffuseMap().lock());
    drawable->getOrCreateUniform(graphics::UniformId::IBLSpecularMap) = makeUniform(IBLLightNodePrivate::defaultSpecularMap().lock());
    drawable->getOrCreateUniform(graphics::UniformId::IBLContribution) = makeUniform(IBLLightNodePrivate::defaultContribution());
}

IBLLightNode::~IBLLightNode() = default;

LightType IBLLightNode::type() const
{
    return LightType::IBL;
}

std::shared_ptr<IBLLightNode> IBLLightNode::asIBLLightNode()
{
    return std::dynamic_pointer_cast<IBLLightNode>(shared_from_this());
}

std::shared_ptr<const IBLLightNode> IBLLightNode::asIBLLightNode() const
{
    return std::dynamic_pointer_cast<const IBLLightNode>(shared_from_this());
}

const graphics::PConstTexture &IBLLightNode::BRDFLutTexture() const
{
    return uniform_cast<graphics::PConstTexture>(m().areaDrawable()->uniform(graphics::UniformId::IBLBRDFLutMap))->data();
}

void IBLLightNode::setBRDFLutTexture(const graphics::PConstTexture &value)
{
    uniform_cast<graphics::PConstTexture>(m().areaDrawable()->uniform(graphics::UniformId::IBLBRDFLutMap))->data() = value;
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

float IBLLightNode::contribution() const
{
    return uniform_cast<float>(m().areaDrawable()->uniform(graphics::UniformId::IBLContribution))->data();
}

void IBLLightNode::setContribution(float value)
{
    uniform_cast<float>(m().areaDrawable()->uniform(graphics::UniformId::IBLContribution))->data() = value;
}

void IBLLightNode::doAfterTransformChanged()
{
    recalculateAreaBoundingBox();
    m().isAreaMatrixDirty() = true;
}

glm::mat4x4 IBLLightNode::doAreaMatrix() const
{
    return glm::mat4x4(1.f); // it is not used because bb policy is Root
}

utils::BoundingBox IBLLightNode::doAreaBoundingBox() const
{
    return utils::BoundingBox::empty(); // it is not used because bb policy is Root
}

glm::mat4x4 IBLLightNode::doUpdateLayeredShadowMatrices(const utils::FrustumCorners &, const utils::Range &, std::vector<glm::mat4x4> &) const
{
    return glm::mat4x4(1.f); // no shadow for IBL
}

}
}

#include <utils/logger.h>

#include <core/applicationbase.h>
#include <core/graphicsengine.h>
#include <core/texturesmanager.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/ibllightnode.h>
#include <core/settings.h>
#include <core/uniform.h>

#include "graphicsengineprivate.h"
#include "ibllightnodeprivate.h"
#include "lightdrawable.h"

namespace simplex
{
namespace core
{

IBLLightNode::IBLLightNode(const std::string &name)
    : LightNode(std::make_unique<IBLLightNodePrivate>(*this, name))
{
    auto app = ApplicationBase::currentApplication();
    if (!app)
        LOG_CRITICAL << "Application can't be nullptr";

    auto graphicsEngine = app->findEngine<GraphicsEngine>("");
    if (!graphicsEngine)
        LOG_CRITICAL << "Graphics engine can't be nullptr";

    auto &mPrivate = m();
    mPrivate.areaDrawable() = std::make_shared<LightDrawable>(graphicsEngine->m().directionalLightAreaVertexArray());

    auto texturesManager = graphicsEngine->texturesManager();

    setBRDFLutMap(texturesManager->loadOrGetDefaultIBLBRDFLutTexture());
    setDuffuseMap(texturesManager->loadOrGetDefaultIBLDiffuseTexture());
    setSpecularMap(texturesManager->loadOrGetDefaultIBLSpecularTexture());
    setContribution(settings::Settings::instance().graphics().ibl().contribution());

    shadow().setMode(ShadingMode::Disabled);
}

IBLLightNode::~IBLLightNode() = default;

LightType IBLLightNode::type() const
{
    return LightType::IBL;
}

std::shared_ptr<IBLLightNode> IBLLightNode::asIBLLightNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<IBLLightNode>(wp.lock());
}

std::shared_ptr<const IBLLightNode> IBLLightNode::asIBLLightNode() const
{
    return const_cast<IBLLightNode*>(this)->asIBLLightNode();
}

graphics::PConstTexture IBLLightNode::BRDFLutMap() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(m().areaDrawable()->uniform(UniformId::IBLBRDFLutMap));
    return uni ? uni->data() : nullptr;
}

void IBLLightNode::setBRDFLutMap(const graphics::PConstTexture &value)
{
    static UniformId s_uniformId = UniformId::IBLBRDFLutMap;

    if (value)
        m().areaDrawable()->getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        m().areaDrawable()->removeUniform(s_uniformId);
}

graphics::PConstTexture IBLLightNode::duffuseMap() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(m().areaDrawable()->uniform(UniformId::IBLDiffuseMap));
    return uni ? uni->data() : nullptr;
}

void IBLLightNode::setDuffuseMap(const graphics::PConstTexture &value)
{
    static UniformId s_uniformId = UniformId::IBLDiffuseMap;

    if (value)
        m().areaDrawable()->getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        m().areaDrawable()->removeUniform(s_uniformId);
}

graphics::PConstTexture IBLLightNode::specularMap() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(m().areaDrawable()->uniform(UniformId::IBLSpecularMap));
    return uni ? uni->data() : nullptr;
}

void IBLLightNode::setSpecularMap(const graphics::PConstTexture &value)
{

    static UniformId s_uniformId = UniformId::IBLSpecularMap;

    if (value)
        m().areaDrawable()->getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        m().areaDrawable()->removeUniform(s_uniformId);
}

float IBLLightNode::contribution() const
{
    auto uni = uniform_cast<float>(m().areaDrawable()->uniform(UniformId::IBLContribution));
    return uni ? uni->data() : settings::Settings::instance().graphics().ibl().contribution();
}

void IBLLightNode::setContribution(float value)
{
    if (value < 0.f)
        LOG_CRITICAL << "IBL contribution value can't be negative";

    m().areaDrawable()->getOrCreateUniform(UniformId::IBLContribution) = makeUniform(value);
}

}
}

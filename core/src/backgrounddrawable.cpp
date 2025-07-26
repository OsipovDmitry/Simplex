#include <utils/logger.h>

#include <core/igraphicswidget.h>
#include <core/graphicsengine.h>
#include <core/texturesmanager.h>
#include <core/settings.h>
#include <core/uniform.h>
#include <core/material.h>

#include "backgrounddrawable.h"

//tmp
#include "graphicsengineprivate.h"

namespace simplex
{
namespace core
{

BackgroundDrawable::BackgroundDrawable()
    : Drawable(nullptr, nullptr)
{
    //tmp
    auto graphicsRenderer = graphics::RendererBase::current();
    if (!graphicsRenderer)
        LOG_CRITICAL << "Graphics renderer can't be nullptr";

    auto graphicsWidget = graphicsRenderer->widget();
    if (!graphicsWidget)
        LOG_CRITICAL << "Graphics widget can't be nullptr";

    auto graphicsEngine = graphicsWidget->graphicsEngine();
    if (!graphicsEngine)
        LOG_CRITICAL << "Graphics engine can't be nullptr";

    setVertexArray(graphicsEngine->m().screenQuadVertexArray());

    auto texturesManager = graphicsEngine->texturesManager();
    setColorMap(texturesManager->loadOrGetDefaultIBLSpecularTexture());

    auto& backgroundSettings = settings::Settings::instance().graphics().background();
    setRoughness(backgroundSettings.roughness());
}

BackgroundDrawable::~BackgroundDrawable() = default;

graphics::PConstTexture BackgroundDrawable::colorMap() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(uniform(UniformId::BackgroundColorMap));
    return uni ? uni->data() : nullptr;
}

void BackgroundDrawable::setColorMap(const graphics::PConstTexture &value)
{
    static UniformId s_uniformId = UniformId::BackgroundColorMap;

    if (value)
        getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        removeUniform(s_uniformId);
}

const glm::vec3 &BackgroundDrawable::color() const
{
    auto uni = uniform_cast<glm::vec3>(uniform(UniformId::BackgroundColor));
    return uni ? uni->data() : settings::Settings::instance().graphics().background().color();
}

void BackgroundDrawable::setColor(const glm::vec3 &value)
{
    getOrCreateUniform(UniformId::BackgroundColor) = makeUniform(value);
}

float BackgroundDrawable::roughness() const
{
    auto uni = uniform_cast<float>(uniform(UniformId::BackgroundRoughness));
    return uni ? uni->data() : settings::Settings::instance().graphics().background().roughness();
}

void BackgroundDrawable::setRoughness(float value)
{
    getOrCreateUniform(UniformId::BackgroundRoughness) = makeUniform(value);
}

const DrawableComponentSet &BackgroundDrawable::componentSet()
{
    static const DrawableComponentSet s_set {
        UniformId::BackgroundColorMap,
        UniformId::BackgroundColor,
        UniformId::BackgroundRoughness
    };
    return s_set;
}


}
}

#include <utils/logger.h>

#include <core/applicationbase.h>
#include <core/graphicsengine.h>
#include <core/texturesmanager.h>
#include <core/settings.h>
#include <core/uniform.h>

#include "backgrounddrawable.h"

namespace simplex
{
namespace core
{

BackgroundDrawable::BackgroundDrawable(const std::shared_ptr<graphics::IVertexArray> &vao)
    : Drawable(vao)
{
    auto app = ApplicationBase::currentApplication();
    if (!app)
        LOG_CRITICAL << "Application can't be nullptr";

    auto graphicsEngine = app->findEngine<GraphicsEngine>("");
    if (!graphicsEngine)
        LOG_CRITICAL << "Graphics engine can't be nullptr";

    auto texturesManager = graphicsEngine->texturesManager();
    setColorMap(texturesManager->loadOrGetDefaultIBLEnvironmentTexture());

    auto &backgroundSettings = settings::Settings::instance().graphics().background();
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

DrawableComponentSet BackgroundDrawable::componentSet()
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

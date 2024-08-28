#include <utils/logger.h>

#include <core/applicationbase.h>
#include <core/graphicsengine.h>
#include <core/background.h>

#include "backgrounddrawable.h"
#include "graphicsengineprivate.h"
#include "backgroundprivate.h"

namespace simplex
{
namespace core
{

Background::Background()
    : m_(std::make_unique<BackgroundPrivate>())
{
    auto app = ApplicationBase::currentApplication();
    if (!app)
        LOG_CRITICAL << "Application can't be nullptr";

    auto graphicsEngine = app->findEngine<GraphicsEngine>("");
    if (!graphicsEngine)
        LOG_CRITICAL << "Graphics engine can't be nullptr";

    m_->drawable() = std::make_unique<BackgroundDrawable>(graphicsEngine->m().screenQuadVertexArray());
}

Background::~Background() = default;

graphics::PConstTexture Background::colorMap() const
{
    return m().drawable()->colorMap();
}

void Background::setColorMap(const graphics::PConstTexture &value)
{
    m().drawable()->setColorMap(value);
}

const glm::vec3 &Background::color() const
{
    return m().drawable()->color();
}

void Background::setColor(const glm::vec3 &value)
{
    m().drawable()->setColor(value);
}

float Background::roughness() const
{
    return m().drawable()->roughness();
}

void Background::setRoughness(float value)
{
    m().drawable()->setRoughness(value);
}



}
}

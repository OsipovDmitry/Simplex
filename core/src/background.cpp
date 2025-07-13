#include <utils/logger.h>

#include <core/igraphicswidget.h>
#include <core/graphicsengine.h>
#include <core/graphicsrendererbase.h>
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
    auto graphicsRenderer = graphics::RendererBase::current();
    if (!graphicsRenderer)
        LOG_CRITICAL << "Graphics renderer can't be nullptr";

    auto graphicsWidget = graphicsRenderer->widget();
    if (!graphicsWidget)
        LOG_CRITICAL << "Graphics widget can't be nullptr";

    auto graphicsEngine = graphicsWidget->graphicsEngine();
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

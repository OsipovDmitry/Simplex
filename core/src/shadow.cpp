#include <utils/logger.h>

#include <core/lightnode.h>
#include <core/settings.h>
#include <core/shadow.h>
#include <core/graphicsrendererbase.h>

#include "lightnodeprivate.h"
#include "shadowprivate.h"
#include "shadowbuffer.h"

namespace simplex
{
namespace core
{

Shadow::Shadow(LightType lightType)
    : m_(std::make_unique<ShadowPrivate>())
{
    switch (lightType)
    {
    case LightType::Point:
    {
        m_->shadowBuffer() = std::make_shared<ShadowBufferCube>();
        break;
    }
    case LightType::Directional:
    case LightType::Spot:
    {
        m_->shadowBuffer() = std::make_shared<ShadowBuffer2D>();
        break;
    }
    default:
        break;
    }

    const auto numLayeredMatrices = LightNodePrivate::numLayeredShadowMatrices(lightType);

    auto layeredMatricesBuffer = ShadowPrivate::LayeredMatricesBuffer::element_type::create();
    layeredMatricesBuffer->resize(numLayeredMatrices);
    layeredMatricesBuffer->setReservedData({ numLayeredMatrices, 0u, 0u, 0u });
    m_->layeredMatricesBuffer() = layeredMatricesBuffer;

    const auto &graphicsSettings = settings::Settings::instance().graphics();
    const auto &shadowSettings = graphicsSettings.shadow();
    setMapSize(glm::uvec2(shadowSettings.mapSize()));
    setMode(shadowSettings.mode());
    setFilter(shadowSettings.filter());
    setDepthBias(shadowSettings.depthBias());
    setCullPlanesLimits(graphicsSettings.cullPlaneLimits());
}

Shadow::~Shadow() = default;

ShadingMode Shadow::mode() const
{
    return m().mode();
}

void Shadow::setMode(ShadingMode value)
{
    m().mode() = value;
}

ShadingFilter Shadow::filter() const
{
    return m().filter();
}

void Shadow::setFilter(ShadingFilter value)
{
    m().filter() = value;
}

float Shadow::depthBias() const
{
    return m().depthBias();
}

void Shadow::setDepthBias(float value)
{
    if (value < 0.f)
        LOG_CRITICAL << "Shadow depth bias can't be less than 0.0";

    m().depthBias() = value;
}

const glm::uvec2 &Shadow::mapSize() const
{
    return m().mapSize();
}

void Shadow::setMapSize(const glm::uvec2 &value)
{
    m().mapSize() = value;
}

const utils::Range &Shadow::cullPlanesLimits() const
{
    return m().cullPlanesLimits();
}

void Shadow::setCullPlanesLimits(const utils::Range &value)
{
    if (value.nearValue() <= 0.f)
        LOG_CRITICAL << "Znear must be greater than 0.0";

    if (value.farValue() <= value.nearValue())
        LOG_CRITICAL << "Zfar must be greater than Znear";

    m().cullPlanesLimits() = value;
}

}
}

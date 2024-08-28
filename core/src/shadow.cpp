#include <utils/logger.h>

#include <core/settings.h>
#include <core/shadow.h>

#include "shadowprivate.h"

namespace simplex
{
namespace core
{

Shadow::Shadow(std::unique_ptr<ShadowPrivate> mPrivate)
    : m_(std::move(mPrivate))
{
    const auto &graphicsSettings = settings::Settings::instance().graphics();
    const auto &shadowSettings = graphicsSettings.shadow();
    setMode(shadowSettings.mode());
    setFilter(shadowSettings.filter());
    setDepthBias(shadowSettings.depthBias());
    setMapSize(glm::uvec2(shadowSettings.mapSize()));
    setCullPlanesLimits(graphicsSettings.cullPlaneLimits());
}

Shadow::~Shadow() = default;

ShadingMode Shadow::mode() const
{
    return m().mode();
}

void Shadow::setMode(ShadingMode value)
{
    auto &mPrivate = m();
    mPrivate.mode() = value;
    mPrivate.frameBuffer() = nullptr; // reset to update
    mPrivate.blur() = nullptr; // reset to update
    mPrivate.layeredMatricesBuffer() = nullptr; // reset to update
}

ShadingFilter Shadow::filter() const
{
    return m().filter();
}

void Shadow::setFilter(ShadingFilter value)
{
    auto &mPrivate = m();
    mPrivate.filter() = value;
    mPrivate.blur() = nullptr; // reset to update
    mPrivate.frameBuffer() = nullptr; // reset to update
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
    auto &mPrivate = m();
    mPrivate.mapSize() = value;
    mPrivate.frameBuffer() = nullptr; // reset to update
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

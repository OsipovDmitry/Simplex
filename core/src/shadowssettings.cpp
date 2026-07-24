#include <utils/logger.h>

#include <core/cameranode.h>
#include <core/settings.h>
#include <core/shadowssettings.h>

#include "cameranodeprivate.h"
#include "renderpipeline.h"
#include "shadowssettingsprivate.h"

namespace simplex
{
namespace core
{

ShadowsSettings::ShadowsSettings(std::unique_ptr<ShadowsSettingsPrivate>&& shadowsSettingsPrivate)
    : m_(std::move(shadowsSettingsPrivate))
{
    const auto& graphicsSettings = settings::Settings::instance().graphics();
    const auto& shadowSettings = graphicsSettings.shadow();

    setFilter(shadowSettings.filter());
    setBlurSigma(shadowSettings.blurSigma());
    setLightBleedingAmount(shadowSettings.lightBleedingAmount());
    setPositiveExponent(shadowSettings.positiveExponent());
    setNegativeExponent(shadowSettings.negativeExponent());
    setMomentsBias(shadowSettings.momentsBias());
    setDepthsBiasFactor(shadowSettings.depthBiasFactor());
    setCascadesBlendDistanceFactor(shadowSettings.cascadesBlendDistanceFactor());
    setCascadesDistancePower(shadowSettings.cascadesDistancePower());
}

ShadowsSettings::~ShadowsSettings() = default;

ShadowFilter ShadowsSettings::filter() const
{
    return m_->filter();
}

void ShadowsSettings::setFilter(ShadowFilter value)
{
    if (m_->filter() != value)
    {
        m_->filter() = value;

        if (auto& renderPipeLine = m_->cameraNode().m().renderPipeLine()) renderPipeLine->setShadowFilter(m_->filter());
    }
}

float ShadowsSettings::blurSigma() const
{
    return m_->blurSigma();
}

void ShadowsSettings::setBlurSigma(float value)
{
    if (m_->blurSigma() != value)
    {
        m_->blurSigma() = value;

        if (auto& renderPipeLine = m_->cameraNode().m().renderPipeLine()) renderPipeLine->setShadowBlurSigma(m_->blurSigma());
    }
}

float ShadowsSettings::lightBleedingAmount() const
{
    return m_->lightBleedingAmount();
}

void ShadowsSettings::setLightBleedingAmount(float value)
{
    if (m_->lightBleedingAmount() != value)
    {
        m_->lightBleedingAmount() = value;

        if (auto& renderPipeLine = m_->cameraNode().m().renderPipeLine())
            renderPipeLine->setShadowLightBleedingAmount(m_->lightBleedingAmount());
    }
}

float ShadowsSettings::positiveExponent() const
{
    return m_->positiveExponent();
}

void ShadowsSettings::setPositiveExponent(float value)
{
    if (m_->positiveExponent() != value)
    {
        m_->positiveExponent() = value;

        if (auto& renderPipeLine = m_->cameraNode().m().renderPipeLine())
            renderPipeLine->setShadowPositiveExponent(m_->positiveExponent());
    }
}

float ShadowsSettings::negativeExponent() const
{
    return m_->negativeExponent();
}

void ShadowsSettings::setNegativeExponent(float value)
{
    if (m_->negativeExponent() != value)
    {
        m_->negativeExponent() = value;

        if (auto& renderPipeLine = m_->cameraNode().m().renderPipeLine())
            renderPipeLine->setShadowNegativeExponent(m_->negativeExponent());
    }
}

float ShadowsSettings::momentsBias() const
{
    return m_->momentsBias();
}

void ShadowsSettings::setMomentsBias(float value)
{
    if (m_->momentsBias() != value)
    {
        m_->momentsBias() = value;

        if (auto& renderPipeLine = m_->cameraNode().m().renderPipeLine()) renderPipeLine->setShadowMomentsBias(m_->momentsBias());
    }
}

float ShadowsSettings::depthBiasFactor() const
{
    return m_->depthBiasFactor();
}

void ShadowsSettings::setDepthsBiasFactor(float value)
{
    if (m_->depthBiasFactor() != value)
    {
        m_->depthBiasFactor() = value;

        if (auto& renderPipeLine = m_->cameraNode().m().renderPipeLine())
            renderPipeLine->setShadowDepthBiasFactor(m_->depthBiasFactor());
    }
}

float ShadowsSettings::cascadesBlendDistanceFactor() const
{
    return m_->cascadesBlendDistanceFactor();
}

void ShadowsSettings::setCascadesBlendDistanceFactor(float value) const
{
    if (value < 0.f)
    {
        LOG_CRITICAL << "Cascades blend distance factor can't be less than 0.0";
        return;
    }

    if (value > 1.f)
    {
        LOG_CRITICAL << "Cascades blend distance factor can't be more than 1.0";
        return;
    }

    if (m_->cascadesBlendDistanceFactor() != value)
    {
        m_->cascadesBlendDistanceFactor() = value;

        if (auto& renderPipeLine = m_->cameraNode().m().renderPipeLine())
            renderPipeLine->setShadowCascadesBlendDistanceFactor(m_->cascadesBlendDistanceFactor());
    }
}

float ShadowsSettings::cascadesDistancePower() const
{
    return m_->cascadesDistancePower();
}

void ShadowsSettings::setCascadesDistancePower(float value) const
{
    if (value <= 0.f)
    {
        LOG_CRITICAL << "Cascades distance power can't be less or equal than 0.0";
        return;
    }

    if (m_->cascadesDistancePower() != value)
    {
        m_->cascadesDistancePower() = value;

        if (auto& renderPipeLine = m_->cameraNode().m().renderPipeLine())
            renderPipeLine->setShadowCascadesDistancePower(m_->cascadesDistancePower());
    }
}

} // namespace core
} // namespace simplex

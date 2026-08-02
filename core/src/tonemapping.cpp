#include <utils/logger.h>

#include <core/cameranode.h>
#include <core/settings.h>
#include <core/tonemapping.h>

#include "cameranodeprivate.h"
#include "renderpipeline.h"
#include "tonemappingprivate.h"

namespace simplex
{
namespace core
{

ToneMapping::ToneMapping(std::unique_ptr<ToneMappingPrivate>&& toneMappingPrivate)
    : m_(std::move(toneMappingPrivate))
{
    const auto& graphicsSettings = settings::Settings::instance().graphics();
    const auto& toneMappingSettings = graphicsSettings.toneMapping();

    setLuminanceRange(toneMappingSettings.luminanceRange());
    setLuminanceClampRange(toneMappingSettings.luminanceClampRange());
    setPixelsFractionToTrim(toneMappingSettings.pixelsFractionToTrim());
    setBaseLuminance(toneMappingSettings.baseLuminance());
    setTauLight(toneMappingSettings.tauLight());
    setTauDark(toneMappingSettings.tauDark());
}

ToneMapping::~ToneMapping() = default;

const utils::Range& ToneMapping::luminanceRange() const
{
    return m_->luminanceRange();
}

void ToneMapping::setLuminanceRange(const utils::Range& value)
{
    if (value.nearValue() <= 0.f)
    {
        LOG_CRITICAL << "Minimum luminance value can't be less or equal than 0.0";
        return;
    }

    if (value.nearValue() >= value.farValue())
    {
        LOG_CRITICAL << "Minimum luminance value can't be greater or equal than maximum luminance value";
        return;
    }

    if (m_->luminanceRange() != value)
    {
        m_->luminanceRange() = value;
        if (auto& renderPipeLine = m_->cameraNode().m().renderPipeLine())
            renderPipeLine->setToneMappingLuminanceRange(m_->luminanceRange());
    }
}

const utils::Range& ToneMapping::luminanceClampRange() const
{
    return m_->luminanceClampRange();
}

void ToneMapping::setLuminanceClampRange(const utils::Range& value)
{
    if (value.nearValue() <= 0.f)
    {
        LOG_CRITICAL << "Minimum luminance clamp value can't be less or equal than 0.0";
        return;
    }

    if (value.nearValue() >= value.farValue())
    {
        LOG_CRITICAL << "Minimum luminance clamp value can't be greater or equal than maximum luminance clamp value";
        return;
    }

    if (m_->luminanceClampRange() != value)
    {
        m_->luminanceClampRange() = value;
        if (auto& renderPipeLine = m_->cameraNode().m().renderPipeLine())
            renderPipeLine->setToneMappingLuminanceClampRange(m_->luminanceClampRange());
    }
}

const std::pair<float, float>& ToneMapping::pixelsFractionToTrim() const
{
    return m_->pixelsFractionToTrim();
}

void ToneMapping::setPixelsFractionToTrim(const std::pair<float, float>& value)
{
    if (value.first < 0.f || value.first >= 1.f)
    {
        LOG_CRITICAL << "Pixels fraction to trim first value can't be outside [0..1)";
        return;
    }

    if (value.second < 0.f || value.second >= 1.f)
    {
        LOG_CRITICAL << "Pixels fraction to trim second value can't be outside [0..1)";
        return;
    }

    if (value.first + value.second >= 1.f)
    {
        LOG_CRITICAL << "Sum of pixels fraction to trim values can't be grater or equal than 1.0";
        return;
    }

    if (m_->pixelsFractionToTrim() != value)
    {
        m_->pixelsFractionToTrim() = value;
        if (auto& renderPipeLine = m_->cameraNode().m().renderPipeLine())
            renderPipeLine->setToneMappingPixelsFractionToTrim(m_->pixelsFractionToTrim());
    }
}

float ToneMapping::tauLight() const
{
    return m_->tauLight();
}

void ToneMapping::setTauLight(float value)
{
    if (value <= 0.f)
    {
        LOG_CRITICAL << "Tau can't be less or equal than 0.0";
        return;
    }

    if (m_->tauLight() != value)
    {
        m_->tauLight() = value;
        if (auto& renderPipeLine = m_->cameraNode().m().renderPipeLine()) renderPipeLine->setToneMappingTauLight(m_->tauLight());
    }
}

float ToneMapping::tauDark() const
{
    return m_->tauDark();
}

void ToneMapping::setTauDark(float value)
{
    if (value <= 0.f)
    {
        LOG_CRITICAL << "Tau can't be less or equal than 0.0";
        return;
    }

    if (m_->tauDark() != value)
    {
        m_->tauDark() = value;
        if (auto& renderPipeLine = m_->cameraNode().m().renderPipeLine()) renderPipeLine->setToneMappingTauDark(m_->tauDark());
    }
}

float ToneMapping::baseLuminance() const
{
    return m_->baseLuminance();
}

void ToneMapping::setBaseLuminance(float value)
{
    if (value <= 0.f)
    {
        LOG_CRITICAL << "Base exposure can't be less or equal than 0.0";
        return;
    }

    if (m_->baseLuminance() != value)
    {
        m_->baseLuminance() = value;
        if (auto& renderPipeLine = m_->cameraNode().m().renderPipeLine())
            renderPipeLine->setToneMappingBaseLumiance(m_->baseLuminance());
    }
}

} // namespace core
} // namespace simplex

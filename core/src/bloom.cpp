#include <core/bloom.h>
#include <core/cameranode.h>
#include <core/settings.h>

#include "bloomprivate.h"
#include "cameranodeprivate.h"
#include "renderpipeline.h"

namespace simplex
{
namespace core
{

Bloom::Bloom(std::unique_ptr<BloomPrivate>&& bloomPrivate)
    : m_(std::move(bloomPrivate))
{
    const auto& graphicsSettings = settings::Settings::instance().graphics();
    const auto& bloomSettings = graphicsSettings.bloom();

    setEnabled(bloomSettings.isEnabled());
    setContribution(bloomSettings.contribution());
    setPassesCount(bloomSettings.passesCount());
}

Bloom::~Bloom() = default;

bool Bloom::isEnabled() const
{
    return m_->isEnabled();
}

void Bloom::setEnabled(bool value)
{
    if (m_->isEnabled() != value)
    {
        m_->isEnabled() = value;

        if (auto& renderPipeLine = m_->cameraNode().m().renderPipeLine()) renderPipeLine->setBloomEnabled(m_->isEnabled());
    }
}

float Bloom::contribution() const
{
    return m_->contribution();
}

void Bloom::setContribution(float value)
{
    if (m_->contribution() != value)
    {
        m_->contribution() = value;

        if (auto& renderPipeLine = m_->cameraNode().m().renderPipeLine())
            renderPipeLine->setBloomContribution(m_->contribution());
    }
}

uint32_t Bloom::passesCount() const
{
    return m_->passesCount();
}

void Bloom::setPassesCount(uint32_t value)
{
    if (m_->passesCount() != value)
    {
        m_->passesCount() = value;

        if (auto& renderPipeLine = m_->cameraNode().m().renderPipeLine()) renderPipeLine->setBloomPassesCount(m_->passesCount());
    }
}

} // namespace core
} // namespace simplex

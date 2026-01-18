#include <core/background.h>
#include <core/material.h>
#include <core/settings.h>

#include "backgroundprivate.h"

#include "resources.h"

namespace simplex
{
namespace core
{

Background::Background()
    : m_(std::make_unique<BackgroundPrivate>())
{
    auto& backgroundSettings = settings::Settings::instance().graphics().background();
    setEnvironmentColor(backgroundSettings.environmentColor());
    setEnvironmentMap(std::make_shared<MaterialMap>(resources::DefaultIBLSpecularMapPath));
    setBlurPower(backgroundSettings.blurPower());
}

Background::~Background() = default;

std::shared_ptr<MaterialMap> Background::environmentMap()
{
    return m_->environmentMap();
}

std::shared_ptr<const MaterialMap> Background::environmentMap() const
{
    return const_cast<Background*>(this)->environmentMap();
}

void Background::setEnvironmentMap(const std::shared_ptr<MaterialMap>& value)
{
    m_->environmentMap() = value;
    m_->onChanged();
}

const glm::quat& Background::rotation() const
{
    return m_->rotation();
}

void Background::setRotation(const glm::quat& value)
{
    m_->rotation() = value;
    m_->onChanged();
}

const glm::vec3& Background::environmentColor() const
{
    return m_->environmentColor();
}

void Background::setEnvironmentColor(const glm::vec3& value) const
{
    m_->environmentColor() = value;
    m_->onChanged();
}

float Background::blurPower() const
{
    return m_->blurPower();
}

void Background::setBlurPower(float value)
{
    m_->blurPower() = value;
    m_->onChanged();
}

}
}

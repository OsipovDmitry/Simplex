#include "backgroundprivate.h"
#include "scenedata.h"

namespace simplex
{
namespace core
{

BackgroundPrivate::BackgroundPrivate()
    : m_rotation()
    , m_environmentColor(1.f)
    , m_blurPower(0.f)
{}

BackgroundPrivate::~BackgroundPrivate() = default;

std::shared_ptr<MaterialMap>& BackgroundPrivate::environmentMap()
{
    return m_environmentMap;
}

glm::quat& BackgroundPrivate::rotation()
{
    return m_rotation;
}

glm::vec3& BackgroundPrivate::environmentColor()
{
    return m_environmentColor;
}

float& BackgroundPrivate::blurPower()
{
    return m_blurPower;
}

std::shared_ptr<BackgroundHandler>& BackgroundPrivate::handler()
{
    return m_handler;
}

void BackgroundPrivate::onChanged()
{
    if (m_handler)
        if (auto sceneData = m_handler->sceneData().lock())
            sceneData->onBackgroundChanged(m_environmentMap, m_rotation, m_environmentColor, m_blurPower);
}

}
}

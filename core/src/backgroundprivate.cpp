#include "backgroundprivate.h"
#include "scenedata.h"

namespace simplex
{
namespace core
{

BackgroundPrivate::BackgroundPrivate()
    : m_environmentColor(1.f)
    , m_blurPower(0.f)
{}

BackgroundPrivate::~BackgroundPrivate() = default;

std::shared_ptr<BackgroundDrawable> &BackgroundPrivate::drawable()
{
    return m_drawable;
}

std::shared_ptr<MaterialMap>& BackgroundPrivate::environmentMap()
{
    return m_environmentMap;
}

glm::vec3& BackgroundPrivate::environmentColor()
{
    return m_environmentColor;
}

float& BackgroundPrivate::blurPower()
{
    return m_blurPower;
}

std::pair<std::shared_ptr<SceneData>, std::shared_ptr<BackgroundHandler>>& BackgroundPrivate::handler()
{
    return m_handler;
}

void BackgroundPrivate::onChanged()
{
    if (m_handler.first)
        m_handler.first->onBackgroundChanged(m_handler.second->background().lock(), m_handler.second->ID());
}

}
}

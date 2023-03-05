#include <utils/logger.h>

#include <core/applicationbase.h>
#include <core/iengine.h>

#include "applicationbaseprivate.h"

namespace simplex
{
namespace core
{

ApplicationBase::ApplicationBase(const std::string &name)
    : m_(std::make_unique<ApplicationBasePrivate>(name))
{
    LOG_INFO << "Application \"" << m_->name() << "\" has been created";
}

ApplicationBase::~ApplicationBase()
{
    while (!m_->engines().empty())
        unregisterEngine(*m_->engines().begin());

    LOG_INFO << "Application \"" << m_->name() << "\" has been destroyed";
}

std::string ApplicationBase::name() const
{
    return m_->name();
}

bool ApplicationBase::registerEngine(std::shared_ptr<IEngine> engine)
{
    if (auto it = m_->engines().find(engine); it != m_->engines().end())
    {
        LOG_ERROR << "Engine \"" << engine->name() << "\" is already registered";
        return false;
    }

    m_->engines().insert(engine);
    LOG_INFO << "Engine \"" << engine->name() << "\" has been registered to application " << name();

    return true;
}

bool ApplicationBase::unregisterEngine(std::shared_ptr<IEngine> engine)
{
    if (auto it = m_->engines().find(engine); it == m_->engines().end())
    {
        LOG_ERROR << "Engine \"" << engine->name() << "\" is not registered";
        return false;
    }

    m_->engines().erase(engine);
    LOG_INFO << "Engine \"" << engine->name() << "\" has been unregistered from application " << name();

    return true;
}

void ApplicationBase::update()
{
    for (auto engine : m_->engines())
        engine->update();

    doUpdate();
}

}
}

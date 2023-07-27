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
    LOG_INFO << "Application \"" << ApplicationBase::name() << "\" has been created";
}

ApplicationBase::~ApplicationBase()
{
    ApplicationBase::shutDown();
    LOG_INFO << "Application \"" << ApplicationBase::name() << "\" has been destroyed";
}

const std::string &ApplicationBase::name() const
{
    return m_->name();
}

std::shared_ptr<IEngine> ApplicationBase::engine(const std::string &name)
{
    auto it = m_->engines().find(name);
    return it == m_->engines().end() ? nullptr : it->second;
}

bool ApplicationBase::registerEngine(std::shared_ptr<IEngine> value)
{
    if (auto eng = engine(value->name()); eng)
    {
        LOG_ERROR << "Engine \"" << eng->name() << "\" is already registered";
        return false;
    }

    m_->engines().insert({value->name(), value});
    LOG_INFO << "Engine \"" << value->name() << "\" has been registered to application \"" << name() << "\"";

    return true;
}

bool ApplicationBase::unregisterEngine(std::shared_ptr<IEngine> value)
{
    if (auto eng = engine(value->name()); !eng)
    {
        LOG_ERROR << "Engine \"" << value->name() << "\" is not registered";
        return false;
    }

    m_->engines().erase(value->name());
    LOG_INFO << "Engine \"" << value->name() << "\" has been unregistered from application \"" << ApplicationBase::name() << "\"";

    return true;
}

void ApplicationBase::shutDown()
{
    while (!m_->engines().empty())
        unregisterEngine(m_->engines().begin()->second);
}

void ApplicationBase::update(uint64_t time, uint32_t dt)
{
    for (const auto &engine : m_->engines())
        engine.second->update(time, dt);

    doUpdate(time, dt);
}

}
}

#include <core/applicationbase.h>
#include <core/iengine.h>

#include "applicationbaseprivate.h"

namespace simplex
{
namespace core
{

ApplicationBase::ApplicationBase()
    : m_(std::make_unique<ApplicationBasePrivate>())
{

}

ApplicationBase::~ApplicationBase()
{
    while (!m_->engines.empty())
        unregisterEngine(*m_->engines.begin());
}

bool ApplicationBase::registerEngine(std::shared_ptr<IEngine> engine)
{
    if (auto it = m_->engines.find(engine); it != m_->engines.end())
    {
        // log...
        return false;
    }

    m_->engines.insert(engine);
    return true;
}

bool ApplicationBase::unregisterEngine(std::shared_ptr<IEngine> engine)
{
    if (auto it = m_->engines.find(engine); it == m_->engines.end())
    {
        // log...
        return false;
    }

    m_->engines.erase(engine);
    return true;
}

void ApplicationBase::update()
{
    for (auto engine : m_->engines)
        engine->update();

    doUpdate();
}

}
}

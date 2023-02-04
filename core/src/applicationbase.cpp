#include <core/applicationbase.h>

#include "applicationbaseprivate.h"

ApplicationBase::ApplicationBase()
    : m_(std::make_unique<ApplicationBasePrivate>())
{

}

bool ApplicationBase::registerEngine(std::shared_ptr<IEngine> engine)
{
    if (auto it = m_->controllers.find(engine); engine != m_->controllers.end())
    {
        // log...
        return false;
    }

    m_->controllers.insert(engine);
    return true;
}

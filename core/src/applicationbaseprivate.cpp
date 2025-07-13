#include "applicationbaseprivate.h"

namespace simplex
{
namespace core
{

std::unique_ptr<ApplicationBase> ApplicationBasePrivate::s_instance;

ApplicationBasePrivate::ApplicationBasePrivate(const std::string &name,
    ApplicationTimeCallback timeCallback,
    ApplicationPollEventsCallback pollEventsCallback)
    : m_name(name)
    , m_timeCallback(timeCallback)
    , m_pollEventsCallback(pollEventsCallback)
    , m_isStopped(true)
    , m_startTime(0u)
    , m_lastUpdateTime(0u)
    , m_lastFPSTime(0u)
    , m_FPSCounter(0u)
{}

std::string &ApplicationBasePrivate::name()
{
    return m_name;
}

debug::Information &ApplicationBasePrivate::debugInformation()
{
    return m_debugInformation;
}

std::list<std::shared_ptr<IDevice>> &ApplicationBasePrivate::devices()
{
    return m_devices;
}

bool& ApplicationBasePrivate::isStopped()
{
    return m_isStopped;
}

uint64_t& ApplicationBasePrivate::startTime()
{
    return m_startTime;
}

uint64_t& ApplicationBasePrivate::lastUpdateTime()
{
    return m_lastUpdateTime;
}

uint64_t& ApplicationBasePrivate::lastFPSTime()
{
    return m_lastFPSTime;
}

uint32_t& ApplicationBasePrivate::FPSCounter()
{
    return m_FPSCounter;
}

ApplicationTimeCallback& ApplicationBasePrivate::timeCallback()
{
    return m_timeCallback;
}

ApplicationPollEventsCallback& ApplicationBasePrivate::pollEventsCallback()
{
    return m_pollEventsCallback;
}

std::unique_ptr<ApplicationBase>& ApplicationBasePrivate::instance()
{
    return s_instance;
}

}
}

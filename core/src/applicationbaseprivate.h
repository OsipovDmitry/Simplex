#ifndef CORE_APPLICATIONBASEPRIVATE_H
#define CORE_APPLICATIONBASEPRIVATE_H

#include <core/applicationbase.h>
#include <core/debuginformation.h>
#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class ApplicationBasePrivate
{
public:
    ApplicationBasePrivate(const std::string&, ApplicationTimeCallback, ApplicationPollEventsCallback);

    std::string& name();
    debug::Information& debugInformation();
    std::list<std::shared_ptr<IDevice>>& devices();
    bool& isStopped();

    uint64_t& startTime();
    uint64_t& lastUpdateTime();
    uint64_t& lastFPSTime();
    uint32_t& FPSCounter();
    uint32_t& FPS();

    ApplicationTimeCallback& timeCallback();
    ApplicationPollEventsCallback& pollEventsCallback();

    std::shared_ptr<Scene>& scene();

    static std::unique_ptr<ApplicationBase>& instance();

private:
    std::string m_name;
    debug::Information m_debugInformation;
    std::list<std::shared_ptr<IDevice>> m_devices;
    bool m_isStopped;

    uint64_t m_startTime;
    uint64_t m_lastUpdateTime;
    uint64_t m_lastFPSTime;
    uint32_t m_FPSCounter;
    uint32_t m_FPS;

    ApplicationTimeCallback m_timeCallback;
    ApplicationPollEventsCallback m_pollEventsCallback;

    std::shared_ptr<Scene> m_scene;

    static std::unique_ptr<ApplicationBase> s_instance;
};

} // namespace core
} // namespace simplex

#endif // CORE_APPLICATIONBASEPRIVATE_H

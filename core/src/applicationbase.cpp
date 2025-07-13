#include <utils/logger.h>

#include <core/applicationbase.h>
#include <core/idevice.h>
#include <core/iengine.h>
#include <core/irenderer.h>
#include <core/settings.h>
#include <core/scene.h>

#include "applicationbaseprivate.h"

namespace simplex
{
namespace core
{

ApplicationBase::~ApplicationBase()
{
    shutDown();
    LOG_INFO << "Application \"" << ApplicationBase::name() << "\" has been destroyed";
}

const std::string &ApplicationBase::name() const
{
    return m_->name();
}

bool ApplicationBase::registerDevice(const std::shared_ptr<IDevice>& value)
{
    if (!value)
    {
        LOG_CRITICAL << "Device can't be nullptr";
        return false;
    }

    auto& devices = m_->devices();
    if (auto it = std::find(devices.begin(), devices.end(), value); it != devices.end())
    {
        LOG_ERROR << "Device \"" << value->name() << "\" has already been registered";
        return false;
    }

    devices.push_back(value);
    LOG_INFO << "Device \"" << value->name() << "\" has been registered to application \"" << ApplicationBase::name() << "\"";

    return true;
}

bool ApplicationBase::unregisterDevice(const std::shared_ptr<IDevice>& value)
{
    if (!value)
    {
        LOG_CRITICAL << "Device can't be nullptr";
        return false;
    }

    auto& devices = m_->devices();
    if (auto it = std::find(devices.begin(), devices.end(), value); it != devices.end())
    {
        devices.erase(it);
        LOG_INFO << "Device \"" << value->name() << "\" has been unregistered from application \"" << ApplicationBase::name() << "\"";
        return true;
    }

    LOG_ERROR << "Device \"" << value->name() << "\" has not been registered yet";
    return false;
}

bool ApplicationBase::isDeviceRegistered(const std::shared_ptr<IDevice> &value)
{
    if (!value)
        return false;

    auto& devices = m_->devices();
    return std::find(devices.begin(), devices.end(), value) != devices.end();
}

void ApplicationBase::run()
{
    m_->isStopped() = false;
    bool hasDevices = true;

    if (!m_->startTime())
    {
        m_->startTime() = m_->timeCallback()();
        m_->lastUpdateTime() = 0u;
    }

    auto &debugInfo = m_->debugInformation();
    debugInfo.scenesInformation.clear();

    while (!m_->isStopped() && hasDevices)
    {
        auto time = m_->timeCallback()() - m_->startTime();
        auto dt = static_cast<uint32_t>(time - m_->lastUpdateTime());
        m_->lastUpdateTime() = time;

        static const uint64_t deltaFps = 1000u;
        ++m_->FPSCounter();
        if (time - m_->lastFPSTime() >= deltaFps)
        {
            auto FPS = static_cast<uint32_t>(static_cast<float>(m_->FPSCounter()) / (0.001f * deltaFps) + 0.5f);
            m_->FPSCounter() = 0u;
            m_->lastFPSTime() = time;

            //LOG_INFO << "FPS: " << FPS;
        }

        hasDevices = false;

        for (auto& device : m_->devices())
        {
            if (!device->isInitialized())
                continue;
            
            hasDevices = true;

            debug::SceneInformation sceneInfo;
            sceneInfo.sceneName = device->name();
            debugInfo.scenesInformation.push_back(sceneInfo);

            device->update(time, dt, sceneInfo);
        }

        if (auto& pollEvents = m_->pollEventsCallback())
            pollEvents();
        
    }
}

void ApplicationBase::stop()
{
    m_->isStopped() = true;
}

bool ApplicationBase::initialize(ApplicationTimeCallback timeCallback, ApplicationPollEventsCallback pollEventsCallback)
{
    if (auto instance = ApplicationBasePrivate::instance().get())
    {
        LOG_WARNING << "Application \"" << instance->name() << "\" has alredy been initialized";
        return true;
    }

    if (!timeCallback || !pollEventsCallback)
        return false;

    ApplicationBasePrivate::instance() = std::unique_ptr<ApplicationBase>(
        new ApplicationBase(settings::Settings::instance().application().name(), timeCallback, pollEventsCallback));
}

ApplicationBase& ApplicationBase::instance()
{
    auto instance = ApplicationBasePrivate::instance().get();
    if (!instance)
    {
        LOG_CRITICAL << "Application has not been initialized yet";
    }

    return *instance;
}

void ApplicationBase::shutDown()
{
    auto& devices = m_->devices();
    while (!devices.empty())
        unregisterDevice(devices.front());
}

//void ApplicationBase::update(uint64_t time, uint32_t dt)
//{
//    auto &debugInfo = m_->debugInformation();
//    debugInfo.scenesInformation.clear();
//
//    std::vector<std::shared_ptr<Scene>> sceneList { m_->scenes().begin(), m_->scenes().end() };
//    std::stable_sort(sceneList.begin(), sceneList.end(), utils::SortedObjectComparator());
//
//    for (auto &scene : sceneList)
//    {
//        debug::SceneInformation sceneInfo;
//        sceneInfo.sceneName = scene->name();
//        debugInfo.scenesInformation.push_back(sceneInfo);
//
//        // update nodes
//        UpdateNodeVisitor nodeUpdateVisitor(time, dt);
//        scene->sceneRootNode()->acceptDown(nodeUpdateVisitor);
//
//        for (const auto &engine : m_->engines())
//            engine->update(renderWidget, scene, time, dt, sceneInfo);
//
//        doUpdate(time, dt);
//    }
//}

ApplicationBase::ApplicationBase(const std::string& name, ApplicationTimeCallback timeCallback, ApplicationPollEventsCallback pollEventsCallback)
    : m_(std::make_unique<ApplicationBasePrivate>(name, timeCallback, pollEventsCallback))
{
    LOG_INFO << "Application \"" << ApplicationBase::name() << "\" has been created";
}

}
}

#include <utils/logger.h>

#include <core/physicsengine.h>
#include <core/settings.h>

#include <physics_bullet/bulletworld.h>

#include "bullet_3_renderer.h"
#include "bulletworldprivate.h"

namespace simplex
{
namespace physics_bullet
{

BulletWorld::~BulletWorld()
{
    const auto& worldName = BulletWorld::name();
    LOG_INFO << "BulletWorld \"" << worldName << "\" has been destroyed";
}

const std::string& BulletWorld::name() const
{
    return m_->name();
}

bool BulletWorld::isInitialized() const
{
    return true; // m_->device() != nullptr;
}

void BulletWorld::update(const std::shared_ptr<core::Scene>& scene, uint64_t time, uint32_t dt)
{
    m_->engine()->update(scene, time, dt);
}

std::shared_ptr<core::IEngine> BulletWorld::engine()
{
    return physicsEngine();
}

std::shared_ptr<const core::IEngine> BulletWorld::engine() const
{
    return physicsEngine();
}

std::shared_ptr<core::PhysicsEngine> BulletWorld::physicsEngine()
{
    return m_->engine();
}

std::shared_ptr<const core::PhysicsEngine> BulletWorld::physicsEngine() const
{
    return const_cast<BulletWorld*>(this)->physicsEngine();
}

std::shared_ptr<BulletWorld> BulletWorld::getOrCreate(const std::string& worldName)
{
    std::shared_ptr<BulletWorld> result;

    auto& instances = BulletWorldPrivate::instances();
    if (auto it =
            instances.find_if([&worldName](const std::shared_ptr<BulletWorld>& value) { return value->name() == worldName; });
        it != instances.end())
    {
        result = it->lock();
    }

    if (!result)
    {
        result = std::shared_ptr<BulletWorld>(new BulletWorld(worldName));
        auto& resultPrivate = result->m();

        auto renderer = std::make_shared<Bullet_3_Renderer>(worldName + "Renderer", result);

        auto engine = std::make_shared<core::PhysicsEngine>(worldName + "Engine", renderer);
        resultPrivate.engine() = engine;

        instances.push_back(result);

        const auto& physicsSettings = core::settings::Settings::instance().physics();
        renderer->makeCurrent();
        renderer->setGravity(physicsSettings.gravity());
        renderer->setSimulationTimeStep(physicsSettings.simulationTimeStep());

        LOG_INFO << "BulletWorld has been created (Name: \"" << worldName << ")";
    }

    return result;
}

BulletWorld::BulletWorld(const std::string& worldName)
    : m_(std::make_unique<BulletWorldPrivate>(worldName))
{
    // auto device = alcOpenDevice(deviceName.c_str());
    // if (!device) LOG_CRITICAL << "Failed to create OpenAL device " << deviceName;

    // m_->device() = device;
}

} // namespace physics_bullet
} // namespace simplex

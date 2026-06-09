#include <utils/logger.h>

#include <core/dynamicbodynode.h>
#include <core/nodecollector.h>
#include <core/physicsengine.h>
#include <core/physicsrendererbase.h>
#include <core/scene.h>
#include <core/scenerootnode.h>

#include "dynamicbodynodeprivate.h"
#include "physicsengineprivate.h"

namespace simplex
{
namespace core
{

PhysicsEngine::PhysicsEngine(const std::string& name, std::shared_ptr<physics::RendererBase> renderer)
    : m_(std::make_unique<PhysicsEnginePrivate>(name))
{
    if (!renderer) LOG_CRITICAL << "Physics renderer can't be nullptr";

    auto& mPrivate = m();

    mPrivate.renderer() = renderer;

    LOG_INFO << "Engine \"" << PhysicsEngine::name() << "\" has been created";
}

PhysicsEngine::~PhysicsEngine()
{
    LOG_INFO << "Engine \"" << PhysicsEngine::name() << "\" has been destroyed";
}

const std::string& PhysicsEngine::name() const
{
    return m_->name();
}

std::shared_ptr<IRenderer> PhysicsEngine::renderer()
{
    return physicsRenderer();
}

std::shared_ptr<const IRenderer> PhysicsEngine::renderer() const
{
    return physicsRenderer();
}

std::shared_ptr<physics::RendererBase> PhysicsEngine::physicsRenderer()
{
    return m_->renderer();
}

std::shared_ptr<const physics::RendererBase> PhysicsEngine::physicsRenderer() const
{
    return const_cast<PhysicsEngine*>(this)->physicsRenderer();
}

void PhysicsEngine::update(const std::shared_ptr<Scene>& scene, uint64_t /*time*/, uint32_t dt)
{
    if (!scene) return;

    auto& renderer = m_->renderer();
    if (!renderer)
    {
        LOG_CRITICAL << "Audio renderer can't be nullptr";
        return;
    }

    renderer->makeCurrent();

    m_->renderer()->stepSimulation(dt);

    NodeCollector<DynamicBodyNode> dynamicBodyNodeCollector;
    scene->sceneRootNode()->acceptDown(dynamicBodyNodeCollector);

    for (const auto& dynamicBodyNode : dynamicBodyNodeCollector.nodes())
    {
        dynamicBodyNode->m().resetBodyTransformChangedAfterSimulation();
    }
}

} // namespace core
} // namespace simplex

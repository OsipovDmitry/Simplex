#ifndef CORE_PHYSICSENGINE_H
#define CORE_PHYSICSENGINE_H

#include <utils/noncopyble.h>
#include <utils/pimpl.h>

#include <core/coreglobal.h>
#include <core/iengine.h>

namespace simplex
{
namespace core
{

class PhysicsEnginePrivate;
class CORE_SHARED_EXPORT PhysicsEngine : public IEngine
{
    NONCOPYBLE(PhysicsEngine)
    PRIVATE_IMPL(PhysicsEngine)

public:
    PhysicsEngine(const std::string&, std::shared_ptr<physics::RendererBase>);
    ~PhysicsEngine() override;

    const std::string& name() const override;

    std::shared_ptr<IRenderer> renderer() override final;
    std::shared_ptr<const IRenderer> renderer() const override final;

    std::shared_ptr<physics::RendererBase> physicsRenderer();
    std::shared_ptr<const physics::RendererBase> physicsRenderer() const;

    void update(const std::shared_ptr<Scene>& scene, uint64_t time, uint32_t dt);

private:
    std::unique_ptr<PhysicsEnginePrivate> m_;
};

} // namespace core
} // namespace simplex

#endif // CORE_PHYSICSENGINE_H

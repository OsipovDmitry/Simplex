#include "bodynodeprivate.h"

#include <core/bodynode.h>
#include <core/physicsrendererbase.h>

namespace simplex
{
namespace core
{

BodyNodePrivate::BodyNodePrivate(BodyNode& bodyNode, const std::string& name)
    : NodePrivate(bodyNode, name)
{
}

BodyNodePrivate::~BodyNodePrivate() = default;

void BodyNodePrivate::onAfterTransformChanged()
{
    body()->setTransform(globalTransform());
}

void BodyNodePrivate::onAttachToScene(const std::shared_ptr<Scene>&)
{
    auto physicsRenderer = physics::RendererBase::current();
    if (!physicsRenderer)
    {
        LOG_CRITICAL << "Physics renderer can't be nullptr";
        return;
    }

    physicsRenderer->addBodyToSimulation(body());
}

void BodyNodePrivate::onDetachFromScene(const std::shared_ptr<Scene>&)
{
    auto physicsRenderer = physics::RendererBase::current();
    if (!physicsRenderer)
    {
        LOG_CRITICAL << "Physics renderer can't be nullptr";
        return;
    }

    physicsRenderer->removeBodyFromSimulation(body());
}

} // namespace core
} // namespace simplex

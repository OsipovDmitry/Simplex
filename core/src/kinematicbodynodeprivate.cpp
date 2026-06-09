#include "kinematicbodynodeprivate.h"

#include <core/kinematicbodynode.h>
#include <core/physicsrendererbase.h>

namespace simplex
{
namespace core
{

KinematicBodyNodePrivate::KinematicBodyNodePrivate(KinematicBodyNode& kineamticBodyNode, const std::string& name)
    : BodyNodePrivate(kineamticBodyNode, name)
{
}

KinematicBodyNodePrivate::~KinematicBodyNodePrivate() = default;

std::shared_ptr<physics::IBody> KinematicBodyNodePrivate::body()
{
    return m_kinematicBody;
}

std::shared_ptr<physics::IKinematicBody>& KinematicBodyNodePrivate::kinematicBody()
{
    return m_kinematicBody;
}

} // namespace core
} // namespace simplex

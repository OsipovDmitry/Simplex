#include "staticbodynodeprivate.h"

#include <core/physicsrendererbase.h>
#include <core/staticbodynode.h>

namespace simplex
{
namespace core
{

StaticBodyNodePrivate::StaticBodyNodePrivate(StaticBodyNode& staticBodyNode, const std::string& name)
    : BodyNodePrivate(staticBodyNode, name)
{
}

StaticBodyNodePrivate::~StaticBodyNodePrivate() = default;

std::shared_ptr<physics::IBody> StaticBodyNodePrivate::body()
{
    return m_staticBody;
}

std::shared_ptr<physics::IStaticBody>& StaticBodyNodePrivate::staticBody()
{
    return m_staticBody;
}

} // namespace core
} // namespace simplex

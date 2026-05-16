#include <core/ambientlightnode.h>

#include "ambientlightnodeprivate.h"

namespace simplex
{
namespace core
{

AmbientLightNode::AmbientLightNode(const std::string& name)
    : LightNode(std::make_unique<AmbientLightNodePrivate>(*this, name))
{
    setColor(glm::vec3(1.f));
}

AmbientLightNode::~AmbientLightNode() = default;

std::shared_ptr<AmbientLightNode> AmbientLightNode::asAmbientLightNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<AmbientLightNode>(wp.lock());
}

std::shared_ptr<const AmbientLightNode> AmbientLightNode::asAmbientLightNode() const
{
    return const_cast<AmbientLightNode*>(this)->asAmbientLightNode();
}

const glm::vec3& AmbientLightNode::color() const
{
    return m().color();
}

void AmbientLightNode::setColor(const glm::vec3& value)
{
    auto& mPrivate = m();

    mPrivate.color() = value;
    mPrivate.onChanged();
}

} // namespace core
} // namespace simplex

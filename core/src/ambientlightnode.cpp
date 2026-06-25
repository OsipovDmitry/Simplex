#include <core/ambientlightnode.h>

#include "ambientlightnodeprivate.h"

namespace simplex
{
namespace core
{

AmbientLightNode::AmbientLightNode(const std::string& name)
    : ColoredLightNode(std::make_unique<AmbientLightNodePrivate>(*this, name))
{
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

} // namespace core
} // namespace simplex

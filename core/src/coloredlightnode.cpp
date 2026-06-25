#include <utils/logger.h>

#include <core/coloredlightnode.h>

#include "coloredlightnodeprivate.h"

namespace simplex
{
namespace core
{

ColoredLightNode::~ColoredLightNode() = default;

std::shared_ptr<ColoredLightNode> ColoredLightNode::asColoredLightNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<ColoredLightNode>(wp.lock());
}

std::shared_ptr<const ColoredLightNode> ColoredLightNode::asColoredLightNode() const
{
    return const_cast<ColoredLightNode*>(this)->asColoredLightNode();
}

std::shared_ptr<AmbientLightNode> ColoredLightNode::asAmbientLightNode()
{
    return nullptr;
}

std::shared_ptr<const AmbientLightNode> ColoredLightNode::asAmbientLightNode() const
{
    return nullptr;
}

std::shared_ptr<ShadowedLightNode> ColoredLightNode::asShadowedLightNode()
{
    return nullptr;
}

std::shared_ptr<const ShadowedLightNode> ColoredLightNode::asShadowedLightNode() const
{
    return nullptr;
}

const glm::vec3& ColoredLightNode::color() const
{
    return m().color();
}

void ColoredLightNode::setColor(const glm::vec3& value)
{
    auto& mPrivate = m();

    mPrivate.color() = value;
    mPrivate.onChanged();
}

ColoredLightNode::ColoredLightNode(std::unique_ptr<ColoredLightNodePrivate> coloredLightNodePrivate)
    : LightNode(std::move(coloredLightNodePrivate))
{
    setColor(glm::vec3(1.f));
}

} // namespace core
} // namespace simplex

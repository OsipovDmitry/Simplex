#ifndef CORE_LIGHTNODE_H
#define CORE_LIGHTNODE_H

#include <utils/enumclass.h>
#include <utils/pimpl.h>

#include <core/node.h>

namespace simplex
{
namespace core
{

ENUMCLASS(LightType, uint16_t, Ambient, Directional, Point, Spot, ImageBased)

class LightNodePrivate;
class CORE_SHARED_EXPORT LightNode : public Node
{
    PRIVATE_IMPL(LightNode)
public:
    ~LightNode() override;

    std::shared_ptr<LightNode> asLightNode() override;
    std::shared_ptr<const LightNode> asLightNode() const override;

    LightType type() const;

    virtual std::shared_ptr<ColoredLightNode> asColoredLightNode();
    virtual std::shared_ptr<const ColoredLightNode> asColoredLightNode() const;

    virtual std::shared_ptr<ImageBasedLightNode> asIBLLightNode();
    virtual std::shared_ptr<const ImageBasedLightNode> asIBLLightNode() const;

    bool isLightingEnabled() const;
    void setLightingEnabled(bool);

protected:
    LightNode(std::unique_ptr<LightNodePrivate>);
};

} // namespace core
} // namespace simplex

#endif // CORE_LIGHTNODE_H

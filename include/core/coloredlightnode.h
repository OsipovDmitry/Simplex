#ifndef CORE_COLOREDLIGHTNODE_H
#define CORE_COLOREDLIGHTNODE_H

#include <utils/glm/vec3.hpp>

#include <core/lightnode.h>

namespace simplex
{
namespace core
{

class ColoredLightNodePrivate;
class CORE_SHARED_EXPORT ColoredLightNode : public LightNode
{
    PRIVATE_IMPL(ColoredLightNode)
public:
    ~ColoredLightNode() override;

    std::shared_ptr<ColoredLightNode> asColoredLightNode() override;
    std::shared_ptr<const ColoredLightNode> asColoredLightNode() const override;

    virtual std::shared_ptr<AmbientLightNode> asAmbientLightNode();
    virtual std::shared_ptr<const AmbientLightNode> asAmbientLightNode() const;

    virtual std::shared_ptr<ShadowedLightNode> asShadowedLightNode();
    virtual std::shared_ptr<const ShadowedLightNode> asShadowedLightNode() const;

    const glm::vec3& color() const;
    void setColor(const glm::vec3&);

protected:
    ColoredLightNode(std::unique_ptr<ColoredLightNodePrivate>);
};

} // namespace core
} // namespace simplex

#endif // CORE_COLOREDLIGHTNODE_H

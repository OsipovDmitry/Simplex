#ifndef CORE_LIGHTNODE_H
#define CORE_LIGHTNODE_H

#include <utils/glm/vec3.hpp>
#include <utils/glm/mat4x4.hpp>
#include <utils/pimpl.h>
#include <utils/enumclass.h>

#include <core/forwarddecl.h>
#include <core/node.h>

namespace simplex
{
namespace core
{

class LightNodePrivate;
class CORE_SHARED_EXPORT LightNode : public Node
{
    PIMPL(LightNode)
public:
    ~LightNode() override;

    std::shared_ptr<LightNode> asLightNode() override;
    std::shared_ptr<const LightNode> asLightNode() const override;

    utils::BoundingBox doBoundingBox() const override;

    virtual std::shared_ptr<PointLightNode> asPointLightNode();
    virtual std::shared_ptr<const PointLightNode> asPointLightNode() const;

    virtual std::shared_ptr<SpotLightNode> asSpotLightNode();
    virtual std::shared_ptr<const SpotLightNode> asSpotLightNode() const;

    virtual std::shared_ptr<DirectionalLightNode> asDirectionalLightNode();
    virtual std::shared_ptr<const DirectionalLightNode> asDirectionalLightNode() const;

    virtual std::shared_ptr<IBLLightNode> asIBLLightNode();
    virtual std::shared_ptr<const IBLLightNode> asIBLLightNode() const;

    bool isLightingEnabled() const;
    void setLightingEnabled(bool);

    bool isShadingEnabled() const;
    void setShadingEnabled(bool);

    const glm::mat4x4 &areaMatrix() const;
    std::shared_ptr<const LightDrawable> areaDrawable() const;
    const utils::BoundingBox &areaBoundingBox() const;
    void recalculateAreaBoundingBox();

    utils::Transform calculateShadowViewTransform(const utils::FrustumCornersInfo&) const;
    glm::mat4x4 calculateShadowProjectionMatrix(const utils::Transform &shadowViewTransform,
                                                const utils::FrustumCornersInfo&,
                                                const utils::Range&) const;

protected:
    LightNode(std::unique_ptr<LightNodePrivate>);

    bool canAttach(std::shared_ptr<Node>) override;
    bool canDetach(std::shared_ptr<Node>) override;

    virtual glm::mat4x4 doAreaMatrix() const = 0;
    virtual utils::BoundingBox doAreaBoundingBox() const = 0;
    virtual utils::Transform doShadowViewTransform(const utils::FrustumCornersInfo&) const /*= 0*/;
    virtual glm::mat4x4 doShadowProjectionMatrix(const utils::Transform &shadowViewTransform,
                                                 const utils::FrustumCornersInfo&,
                                                 const utils::Range&) const /*= 0*/;
};

}
}

#endif // CORE_LIGHTNODE_H

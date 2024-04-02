#ifndef CORE_IBLLIGHTNODE_H
#define CORE_IBLLIGHTNODE_H

#include <core/lightnode.h>

namespace simplex
{
namespace core
{

class IBLLightNodePrivate;
class CORE_SHARED_EXPORT IBLLightNode : public LightNode
{
    PIMPL(IBLLightNode)
public:
    IBLLightNode(const std::string&);
    ~IBLLightNode() override;

    LightType type() const override;

    std::shared_ptr<IBLLightNode> asIBLLightNode() override;
    std::shared_ptr<const IBLLightNode> asIBLLightNode() const override;

    const graphics::PConstTexture &BRDFLutTexture() const;
    void setBRDFLutTexture(const graphics::PConstTexture&);

    const graphics::PConstTexture &duffuseTexture() const;
    void setDuffuseTexture(const graphics::PConstTexture&);

    const graphics::PConstTexture &specularTexture() const;
    void setSpecularTexture(const graphics::PConstTexture&);

    float contribution() const;
    void setContribution(float);

    void doAfterTransformChanged() override;

protected:
    glm::mat4x4 doAreaMatrix() const override;
    utils::BoundingBox doAreaBoundingBox() const override;
    glm::mat4x4 doUpdateLayeredShadowMatrices(const utils::FrustumCorners&,
                                              const utils::Range&,
                                              std::vector<glm::mat4x4>&) const override;

};

}
}

#endif // CORE_IBLLIGHTNODE_H

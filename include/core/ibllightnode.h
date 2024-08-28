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
    PRIVATE_IMPL(IBLLightNode)
public:
    IBLLightNode(const std::string&);
    ~IBLLightNode() override;

    LightType type() const override;

    std::shared_ptr<IBLLightNode> asIBLLightNode() override;
    std::shared_ptr<const IBLLightNode> asIBLLightNode() const override;

    graphics::PConstTexture BRDFLutMap() const;
    void setBRDFLutMap(const graphics::PConstTexture&);

    graphics::PConstTexture duffuseMap() const;
    void setDuffuseMap(const graphics::PConstTexture&);

    graphics::PConstTexture specularMap() const;
    void setSpecularMap(const graphics::PConstTexture&);

    float contribution() const;
    void setContribution(float);

};

}
}

#endif // CORE_IBLLIGHTNODE_H

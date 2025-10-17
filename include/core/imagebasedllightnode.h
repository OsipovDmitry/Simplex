#ifndef CORE_IBLLIGHTNODE_H
#define CORE_IBLLIGHTNODE_H

#include <core/lightnode.h>

namespace simplex
{
namespace core
{

class ImageBasedLightNodePrivate;
class CORE_SHARED_EXPORT ImageBasedLightNode : public LightNode
{
    PRIVATE_IMPL(ImageBasedLightNode)
public:
    ImageBasedLightNode(const std::string&);
    ~ImageBasedLightNode() override;

    std::shared_ptr<ImageBasedLightNode> asIBLLightNode() override;
    std::shared_ptr<const ImageBasedLightNode> asIBLLightNode() const override;

    std::shared_ptr<MaterialMap> BRDFLutMap();
    std::shared_ptr<const MaterialMap> BRDFLutMap() const;
    void setBRDFLutMap(const std::shared_ptr<MaterialMap>&);

    std::shared_ptr<MaterialMap> diffuseMap();
    std::shared_ptr<const MaterialMap> diffuseMap() const;
    void setDiffuseMap(const std::shared_ptr<MaterialMap>&);

    std::shared_ptr<MaterialMap> specularMap();
    std::shared_ptr<const MaterialMap> specularMap() const;
    void setSpecularMap(const std::shared_ptr<MaterialMap>&);

    float contribution() const;
    void setContribution(float);

};

}
}

#endif // CORE_IBLLIGHTNODE_H

#ifndef CORE_BONENODE_H
#define CORE_BONENODE_H

#include <core/node.h>

namespace simplex
{
namespace core
{

class BoneNodePrivate;
class CORE_SHARED_EXPORT BoneNode : public Node
{
    PRIVATE_IMPL(BoneNode)
public:
    BoneNode(const std::string&, uint32_t);
    ~BoneNode() override;

    std::shared_ptr<BoneNode> asBoneNode() override;
    std::shared_ptr<const BoneNode> asBoneNode() const override;

    uint32_t boneID();

    virtual std::shared_ptr<SkeletalAnimatedNode> asSkeletalAnimatedNode();
    virtual std::shared_ptr<const SkeletalAnimatedNode> asSkeletalAnimatedNode() const;

protected:
    BoneNode(std::unique_ptr<BoneNodePrivate>&&);
};

}
}

#endif // CORE_BONENODE_H

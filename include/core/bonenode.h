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
    BoneNode(const std::string&, uint32_t, const utils::Transform&);
    ~BoneNode() override;

    std::shared_ptr<BoneNode> asBoneNode() override;
    std::shared_ptr<const BoneNode> asBoneNode() const override;

    uint32_t boneID();
    const utils::Transform& offsetTransform() const;
};

}
}

#endif // CORE_BONENODE_H

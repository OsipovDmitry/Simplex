#ifndef CORE_SKELETALANIMATEDNODE_H
#define CORE_SKELETALANIMATEDNODE_H

#include <core/node.h>

namespace simplex
{
namespace core
{

class SkeletalAnimatedNodePrivate;
class CORE_SHARED_EXPORT SkeletalAnimatedNode : public Node
{
    PRIVATE_IMPL(SkeletalAnimatedNode)
public:
    SkeletalAnimatedNode(const std::string& name, const std::shared_ptr<Skeleton>& skeleton);
    ~SkeletalAnimatedNode() override;

    std::shared_ptr<SkeletalAnimatedNode> asSkeletalAnimatedNode() override;
    std::shared_ptr<const SkeletalAnimatedNode> asSkeletalAnimatedNode() const override;

    const std::shared_ptr<Skeleton>& skeleton() const;

    const std::string& currentAnimation() const;
    bool setCurrentAnimation(const std::string&);
};

} // namespace core
} // namespace simplex

#endif // CORE_SKELETALANIMATEDNODE_H

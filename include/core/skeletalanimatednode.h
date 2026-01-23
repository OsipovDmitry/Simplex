#ifndef CORE_SKELETALANIMATEDNODE_H
#define CORE_SKELETALANIMATEDNODE_H

#include <map>

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
    SkeletalAnimatedNode(const std::string&);
    ~SkeletalAnimatedNode() override;

    std::shared_ptr<SkeletalAnimatedNode> asSkeletalAnimatedNode() override;
    std::shared_ptr<const SkeletalAnimatedNode> asSkeletalAnimatedNode() const override;

    const std::map<std::string, std::shared_ptr<Animation>>& animations() const;
    bool addAnimation(const std::shared_ptr<Animation>&);
    bool removeAnimation(const std::string&);

    const std::shared_ptr<Animation> currentAnimation() const;
    bool setCurrentAnimation(const std::string&);
};

}
}

#endif // CORE_SKELETALANIMATEDNODE_H

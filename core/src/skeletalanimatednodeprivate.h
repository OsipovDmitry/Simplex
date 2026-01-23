#ifndef CORE_SKELETALANIMATEDNODEPRIVATE_H
#define CORE_SKELETALANIMATEDNODEPRIVATE_H

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class SkeletalAnimatedNodePrivate : public NodePrivate
{
public:
    SkeletalAnimatedNodePrivate(SkeletalAnimatedNode&, const std::string&);
    ~SkeletalAnimatedNodePrivate() override;

    std::map<std::string, std::shared_ptr<Animation>>& animations();
    std::shared_ptr<Animation>& currentAnimation();

private:
    std::map<std::string, std::shared_ptr<Animation>> m_animations;
    std::shared_ptr<Animation> m_currentAnimation;

};

}
}

#endif // CORE_CAMERANODEPRIVATE_H

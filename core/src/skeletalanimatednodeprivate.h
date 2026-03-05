#ifndef CORE_SKELETALANIMATEDNODEPRIVATE_H
#define CORE_SKELETALANIMATEDNODEPRIVATE_H

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class SkeletalAnimatedDataHandler;

class SkeletalAnimatedNodePrivate : public NodePrivate
{
public:
    SkeletalAnimatedNodePrivate(SkeletalAnimatedNode&, const std::string&, const std::shared_ptr<Skeleton>&);
    ~SkeletalAnimatedNodePrivate() override;

    void doAttachToScene(const std::shared_ptr<Scene>&) override;
    void doDetachFromScene(const std::shared_ptr<Scene>&) override;

    std::shared_ptr<Skeleton>& skeleton();
    std::string& currentAnimation();

    std::shared_ptr<SkeletalAnimatedDataHandler>& handler();
    void onChanged();

private:
    std::shared_ptr<Skeleton> m_skeleton;
    std::string m_currentAnimation;

    std::shared_ptr<SkeletalAnimatedDataHandler> m_handler;
};

} // namespace core
} // namespace simplex

#endif // CORE_CAMERANODEPRIVATE_H

#include <core/scene.h>
#include <core/skeletalanimatednode.h>

#include "scenedata.h"
#include "sceneprivate.h"
#include "skeletalanimatednodeprivate.h"

namespace simplex
{
namespace core
{

SkeletalAnimatedNodePrivate::SkeletalAnimatedNodePrivate(
    SkeletalAnimatedNode &skeletalAnimatedNode,
    const std::string &name,
    uint32_t boneID,
    const std::shared_ptr<Skeleton>& skeleton)
    : BoneNodePrivate(skeletalAnimatedNode, name, boneID)
    , m_skeleton(skeleton)
{
}

SkeletalAnimatedNodePrivate::~SkeletalAnimatedNodePrivate() = default;

void SkeletalAnimatedNodePrivate::doAttachToScene(const std::shared_ptr<Scene>& scene)
{
    if (const auto& sceneData = scene->m().sceneData())
        m_handler = sceneData->addSkeletalAnimatedData(m_skeleton, m_currentAnimation);
}

void SkeletalAnimatedNodePrivate::doDetachFromScene(const std::shared_ptr<Scene>&)
{
    m_handler.reset();
}

std::shared_ptr<Skeleton>& SkeletalAnimatedNodePrivate::skeleton()
{
    return m_skeleton;
}

std::string& SkeletalAnimatedNodePrivate::currentAnimation()
{
    return m_currentAnimation;
}

std::shared_ptr<SkeletalAnimatedDataHandler>& SkeletalAnimatedNodePrivate::handler()
{
    return m_handler;
}

void SkeletalAnimatedNodePrivate::onChanged()
{
    if (m_handler)
    {
        if (auto sceneData = m_handler->sceneData().lock())
            sceneData->onSkeletalAnimatedDataChanged(m_handler->ID(), m_skeleton, m_currentAnimation);
    }

}

}
}

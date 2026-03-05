#include <utils/logger.h>

#include <core/drawable.h>
#include <core/drawablenode.h>
#include <core/scene.h>

#include "drawablenodeprivate.h"
#include "sceneprivate.h"

namespace simplex
{
namespace core
{

DrawableNode::DrawableNode(const std::string& name, const std::weak_ptr<SkeletalAnimatedNode>& skeletalAnimatedNode)
    : Node(std::make_unique<DrawableNodePrivate>(*this, name, skeletalAnimatedNode))
{
}

DrawableNode::~DrawableNode() = default;

std::shared_ptr<DrawableNode> DrawableNode::asDrawableNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<DrawableNode>(wp.lock());
}

std::shared_ptr<const DrawableNode> DrawableNode::asDrawableNode() const
{
    return const_cast<DrawableNode*>(this)->asDrawableNode();
}

std::weak_ptr<SkeletalAnimatedNode> DrawableNode::skeletalAnimatedNode()
{
    return m().skeletalAnimatedNode();
}

std::weak_ptr<const SkeletalAnimatedNode> DrawableNode::skeletalAnimatedNode() const
{
    return const_cast<DrawableNode*>(this)->skeletalAnimatedNode();
}

const std::unordered_set<std::shared_ptr<Drawable>>& DrawableNode::drawables() const
{
    return m().drawables();
}

void DrawableNode::addDrawable(const std::shared_ptr<Drawable>& drawable)
{
    if (!drawable)
    {
        LOG_ERROR << "Drawable can't be nullptr";
        return;
    }

    auto& mPrivate = m();
    mPrivate.drawables().insert(drawable);

    if (auto s = scene())
        if (auto& sceneData = s->m().sceneData()) mPrivate.addDrawDataToSceneData(sceneData, drawable);
}

void DrawableNode::removeDrawable(const std::shared_ptr<Drawable>& drawable)
{
    auto& mPrivate = m();
    mPrivate.drawables().erase(drawable);
    mPrivate.removeDrawDataFromSceneData(drawable);
}

} // namespace core
} // namespace simplex

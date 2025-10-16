#include <utils/logger.h>

#include <core/scene.h>
#include <core/visualdrawablenode.h>
#include <core/visualdrawable.h>

#include "sceneprivate.h"
#include "visualdrawablenodeprivate.h"

namespace simplex
{
namespace core
{

VisualDrawableNode::VisualDrawableNode(const std::string &name)
    : Node(std::make_unique<VisualDrawableNodePrivate>(*this, name))
{
}

VisualDrawableNode::~VisualDrawableNode() = default;

std::shared_ptr<VisualDrawableNode> VisualDrawableNode::asVisualDrawableNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<VisualDrawableNode>(wp.lock());
}

std::shared_ptr<const VisualDrawableNode> VisualDrawableNode::asVisualDrawableNode() const
{
    return const_cast<VisualDrawableNode*>(this)->asVisualDrawableNode();
}

const std::unordered_set<std::shared_ptr<VisualDrawable>> &VisualDrawableNode::visualDrawables() const
{
    return m().visualDrawables();
}

void VisualDrawableNode::addVisualDrawable(const std::shared_ptr<VisualDrawable>& drawable)
{
    if (!drawable)
    {
        LOG_ERROR << "Drawable can't be nullptr";
        return;
    }

    auto &mPrivate = m();
    mPrivate.visualDrawables().insert(drawable);
    mPrivate.dirtyLocalBoundingBox();

    if (auto s = scene())
        if (auto& sceneData = s->m().sceneData())
            mPrivate.addDrawDataToSceneData(sceneData, drawable); // to add draw data after marking local BB as dirty
}

void VisualDrawableNode::removeVisualDrawable(const std::shared_ptr<VisualDrawable>& drawable)
{
    auto &mPrivate = m();
    mPrivate.visualDrawables().erase(drawable);
    mPrivate.dirtyLocalBoundingBox();

    mPrivate.removeDrawDataFromSceneData(drawable);

}

}
}

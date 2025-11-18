#include <utils/logger.h>

#include <core/scene.h>
#include <core/drawablenode.h>
#include <core/drawable.h>

#include "sceneprivate.h"
#include "drawablenodeprivate.h"

namespace simplex
{
namespace core
{

DrawableNode::DrawableNode(const std::string &name)
    : Node(std::make_unique<DrawableNodePrivate>(*this, name))
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

const std::unordered_set<std::shared_ptr<Drawable>> &DrawableNode::drawables() const
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

    auto &mPrivate = m();
    mPrivate.drawables().insert(drawable);

    if (auto s = scene())
        if (auto& sceneData = s->m().sceneData())
            mPrivate.addDrawDataToSceneData(sceneData, drawable);
}

void DrawableNode::removeDrawable(const std::shared_ptr<Drawable>& drawable)
{
    auto &mPrivate = m();
    mPrivate.drawables().erase(drawable);
    mPrivate.removeDrawDataFromSceneData(drawable);

}

}
}

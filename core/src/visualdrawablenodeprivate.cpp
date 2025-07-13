#include <core/graphicsrendererbase.h>
#include <core/visualdrawablenode.h>
#include <core/visualdrawable.h>

#include "visualdrawablenodeprivate.h"

namespace simplex
{
namespace core
{

VisualDrawableNodePrivate::VisualDrawableNodePrivate(VisualDrawableNode &visualDrawableNode, const std::string &name)
    : NodePrivate(visualDrawableNode, name)
    , m_isLocalBoundingBoxDirty(true)
{}

VisualDrawableNodePrivate::~VisualDrawableNodePrivate() = default;

utils::BoundingBox VisualDrawableNodePrivate::doBoundingBox()
{
    return localBoundingBox();
}

std::unordered_set<std::shared_ptr<VisualDrawable>> &VisualDrawableNodePrivate::visualDrawables()
{
    return m_visualDrawables;
}

std::shared_ptr<Skeleton> &VisualDrawableNodePrivate::skeleton()
{
    return m_skeleton;
}

const utils::BoundingBox &VisualDrawableNodePrivate::localBoundingBox()
{
    if (m_isLocalBoundingBoxDirty)
    {
        utils::BoundingBox bb;
        for (const auto &drawable : visualDrawables())
            bb += drawable->boundingBox();

        m_localBoundingBox = bb;
        m_isLocalBoundingBoxDirty = false;
    }
    return m_localBoundingBox;
}

void VisualDrawableNodePrivate::dirtyLocalBoundingBox()
{
    m_isLocalBoundingBoxDirty = true;
    dirtyBoundingBox();
}

void VisualDrawableNodePrivate::update(const std::shared_ptr<graphics::RendererBase> &graphicsRenderer)
{

}

}
}

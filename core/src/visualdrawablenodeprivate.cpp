#include <core/graphicsrendererbase.h>
#include <core/visualdrawablenode.h>
#include <core/visualdrawable.h>
#include <core/scene.h>

#include "visualdrawablenodeprivate.h"
#include "sceneprivate.h"
#include "scenedata.h"

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

void VisualDrawableNodePrivate::doAfterTransformChanged()
{
    if (auto s = d().scene())
    {
        for (const auto& visualDrawable : m_visualDrawables)
            addDrawDataToScene(s, visualDrawable);
    }
}

void VisualDrawableNodePrivate::doAttachToScene(const std::shared_ptr<Scene>& scene)
{
    for (const auto& visualDrawable : m_visualDrawables)
        addDrawDataToScene(scene, visualDrawable);
}

void VisualDrawableNodePrivate::doDetachFromScene(const std::shared_ptr<Scene>& scene)
{
    for (const auto& visualDrawable : m_visualDrawables)
        removeDrawDataFromScene(scene, visualDrawable);
}

void VisualDrawableNodePrivate::addDrawDataToScene(
    const std::shared_ptr<Scene>& scene,
    const std::shared_ptr<const VisualDrawable>& drawable)
{
    if (!scene)
    {
        LOG_ERROR << "Scene can't be nullptr";
        return;
    }

    if (!drawable)
    {
        LOG_ERROR << "Drawable can't be nullptr";
        return;
    }

    if (auto it = m_handlers.find(drawable); it == m_handlers.end())
    {
        auto handler = scene->m().sceneData()->addDrawData(drawable, d().globalTransform());
        m_handlers.insert({ drawable, handler });
    }
}

void VisualDrawableNodePrivate::removeDrawDataFromScene(
    const std::shared_ptr<Scene>& scene,
    const std::shared_ptr<const VisualDrawable>& drawable)
{
    if (!scene)
    {
        LOG_ERROR << "Scene can't be nullptr";
        return;
    }

    if (!drawable)
    {
        LOG_ERROR << "Drawable can't be nullptr";
        return;
    }

    if (auto it = m_handlers.find(drawable); it != m_handlers.end())
    {
        auto& handler = it->second;
        scene->m().sceneData()->removeDrawData(handler->ID());
        m_handlers.erase(drawable);
    }
    else
    {
        LOG_ERROR << "No found draw data handler to remove";
        return;
    }
}

std::unordered_set<std::shared_ptr<const VisualDrawable>> &VisualDrawableNodePrivate::visualDrawables()
{
    return m_visualDrawables;
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

}
}

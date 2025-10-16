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
    changeDrawDataInSceneData();
}

void VisualDrawableNodePrivate::doAttachToScene(const std::shared_ptr<Scene>& scene)
{
    if (auto sceneData = scene->m().sceneData())
        for (const auto& visualDrawable : m_visualDrawables)
            addDrawDataToSceneData(sceneData, visualDrawable);
}

void VisualDrawableNodePrivate::doDetachFromScene(const std::shared_ptr<Scene>& scene)
{
    for (const auto& visualDrawable : m_visualDrawables)
        removeDrawDataFromSceneData(visualDrawable);
}

void VisualDrawableNodePrivate::addDrawDataToSceneData(
    const std::shared_ptr<SceneData>& sceneData,
    const std::shared_ptr<const VisualDrawable>& drawable)
{
    if (!sceneData)
    {
        LOG_ERROR << "Scene data can't be nullptr";
        return;
    }

    if (!drawable)
    {
        LOG_ERROR << "Drawable can't be nullptr";
        return;
    }
    
    auto handler = sceneData->addDrawData(drawable, d().globalTransform());
    m_handlers.insert({ drawable, handler });
}

void VisualDrawableNodePrivate::removeDrawDataFromSceneData(const std::shared_ptr<const VisualDrawable>& drawable)
{
    if (!drawable)
    {
        LOG_ERROR << "Drawable can't be nullptr";
        return;
    }

    if (auto it = m_handlers.find(drawable); it == m_handlers.end())
    {
        LOG_ERROR << "No found draw data handler to remove";
        return;
    }
    else
    {
        m_handlers.erase(it);
    }
}

void VisualDrawableNodePrivate::changeDrawDataInSceneData()
{
    const auto& globalTranform = d().globalTransform();

    for (const auto& handler : m_handlers)
    {
        if (auto sceneData = handler.second->sceneData().lock())
            sceneData->onDrawDataChanged(handler.second->ID(), handler.first, globalTranform);
    }
}

std::unordered_set<std::shared_ptr<VisualDrawable>> &VisualDrawableNodePrivate::visualDrawables()
{
    return m_visualDrawables;
}

std::unordered_map<std::shared_ptr<const VisualDrawable>, std::shared_ptr<DrawDataHandler>>& VisualDrawableNodePrivate::handlers()
{
    return m_handlers;
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

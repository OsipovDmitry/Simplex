#include <core/graphicsrendererbase.h>
#include <core/drawablenode.h>
#include <core/drawable.h>
#include <core/scene.h>
#include <core/mesh.h>

#include "drawablenodeprivate.h"
#include "sceneprivate.h"
#include "scenedata.h"

namespace simplex
{
namespace core
{

DrawableNodePrivate::DrawableNodePrivate(DrawableNode &drawableNode, const std::string &name)
    : NodePrivate(drawableNode, name)
    , m_isLocalBoundingBoxDirty(true)
{}

DrawableNodePrivate::~DrawableNodePrivate() = default;

utils::BoundingBox DrawableNodePrivate::doBoundingBox()
{
    return localBoundingBox();
}

void DrawableNodePrivate::doAfterTransformChanged()
{
    changeDrawDataInSceneData();
}

void DrawableNodePrivate::doAttachToScene(const std::shared_ptr<Scene>& scene)
{
    if (auto sceneData = scene->m().sceneData())
        for (const auto& drawable : m_drawables)
            addDrawDataToSceneData(sceneData, drawable);
}

void DrawableNodePrivate::doDetachFromScene(const std::shared_ptr<Scene>& scene)
{
    for (const auto& drawable : m_drawables)
        removeDrawDataFromSceneData(drawable);
}

void DrawableNodePrivate::addDrawDataToSceneData(
    const std::shared_ptr<SceneData>& sceneData,
    const std::shared_ptr<const Drawable>& drawable)
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

void DrawableNodePrivate::removeDrawDataFromSceneData(const std::shared_ptr<const Drawable>& drawable)
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

void DrawableNodePrivate::changeDrawDataInSceneData()
{
    const auto& globalTranform = d().globalTransform();

    for (const auto& handler : m_handlers)
    {
        if (auto sceneData = handler.second->sceneData().lock())
            sceneData->onDrawDataChanged(handler.second->ID(), handler.first, globalTranform);
    }
}

std::unordered_set<std::shared_ptr<Drawable>> &DrawableNodePrivate::drawables()
{
    return m_drawables;
}

std::unordered_map<std::shared_ptr<const Drawable>, std::shared_ptr<DrawDataHandler>>& DrawableNodePrivate::handlers()
{
    return m_handlers;
}

const utils::BoundingBox &DrawableNodePrivate::localBoundingBox()
{
    if (m_isLocalBoundingBoxDirty)
    {
        utils::BoundingBox bb;
        for (const auto &drawable : drawables())
            if (auto mesh = drawable->mesh())
            bb += mesh->boundingBox();

        m_localBoundingBox = bb;
        m_isLocalBoundingBoxDirty = false;
    }
    return m_localBoundingBox;
}

void DrawableNodePrivate::dirtyLocalBoundingBox()
{
    m_isLocalBoundingBoxDirty = true;
    dirtyBoundingBox();
}

}
}

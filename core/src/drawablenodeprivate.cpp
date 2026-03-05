#include "drawablenodeprivate.h"

#include <core/drawable.h>
#include <core/drawablenode.h>
#include <core/graphicsrendererbase.h>
#include <core/mesh.h>
#include <core/scene.h>
#include <core/skeletalanimatednode.h>

#include "scenedata.h"
#include "sceneprivate.h"
#include "skeletalanimatednodeprivate.h"

namespace simplex
{
namespace core
{

DrawableNodePrivate::DrawableNodePrivate(
    DrawableNode& drawableNode,
    const std::string& name,
    const std::weak_ptr<SkeletalAnimatedNode>& skeletalAnimatedNode)
    : NodePrivate(drawableNode, name)
    , m_skeletalAnimatedNode(skeletalAnimatedNode)
{
}

DrawableNodePrivate::~DrawableNodePrivate() = default;

void DrawableNodePrivate::doAfterTransformChanged()
{
    changeDrawDataInSceneData();
}

void DrawableNodePrivate::doAttachToScene(const std::shared_ptr<Scene>& scene)
{
    if (const auto& sceneData = scene->m().sceneData())
        for (const auto& drawable : m_drawables)
            addDrawDataToSceneData(sceneData, drawable);
}

void DrawableNodePrivate::doDetachFromScene(const std::shared_ptr<Scene>&)
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

    auto handler = sceneData->addDrawData(drawable, d().globalTransform(), skeletalAnimatedDataID(sceneData));
    m_handlers.insert({drawable, handler});
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
            sceneData->onDrawDataChanged(handler.second->ID(), handler.first, globalTranform, skeletalAnimatedDataID(sceneData));
    }
}

std::unordered_set<std::shared_ptr<Drawable>>& DrawableNodePrivate::drawables()
{
    return m_drawables;
}

std::weak_ptr<SkeletalAnimatedNode>& DrawableNodePrivate::skeletalAnimatedNode()
{
    return m_skeletalAnimatedNode;
}

std::unordered_map<std::shared_ptr<const Drawable>, std::shared_ptr<DrawDataHandler>>& DrawableNodePrivate::handlers()
{
    return m_handlers;
}

utils::IDsGenerator::value_type DrawableNodePrivate::skeletalAnimatedDataID(const std::shared_ptr<SceneData>& sceneData) const
{
    if (auto skeletalAnimatedNode = m_skeletalAnimatedNode.lock())
        if (auto& skeletalAnimatedDataHandler = skeletalAnimatedNode->m().handler())
            if (sceneData == skeletalAnimatedDataHandler->sceneData().lock()) return skeletalAnimatedDataHandler->ID();

    return utils::IDsGenerator::last();
}

} // namespace core
} // namespace simplex

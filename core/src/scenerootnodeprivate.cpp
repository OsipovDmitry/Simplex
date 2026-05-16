#include "scenerootnodeprivate.h"

#include <utils/logger.h>

#include <core/scenerootnode.h>

namespace simplex
{
namespace core
{

SceneRootNodePrivate::SceneRootNodePrivate(SceneRootNode& sceneRootNode, const std::string& name)
    : NodePrivate(sceneRootNode, name)
{
}

SceneRootNodePrivate::~SceneRootNodePrivate() = default;

void SceneRootNodePrivate::onAttachToParent(const std::shared_ptr<Node>&)
{
    LOG_CRITICAL << "SceneRootNode can't be attached";
}

void SceneRootNodePrivate::onDetachFromParent(const std::shared_ptr<Node>&)
{
    LOG_CRITICAL << "SceneRootNode can't be detached";
}

std::weak_ptr<Scene>& SceneRootNodePrivate::scene()
{
    return m_scene;
}

} // namespace core
} // namespace simplex

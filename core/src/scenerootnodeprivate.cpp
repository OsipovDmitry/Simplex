#include <utils/logger.h>

#include <core/scenerootnode.h>

#include "scenerootnodeprivate.h"

namespace simplex
{
namespace core
{

SceneRootNodePrivate::SceneRootNodePrivate(SceneRootNode &sceneRootNode, const std::string &name)
    : NodePrivate(sceneRootNode, name)
{
}

SceneRootNodePrivate::~SceneRootNodePrivate() = default;

void SceneRootNodePrivate::doAttachToParent()
{
    LOG_CRITICAL << "SceneRootNode can't be attached";
}

void SceneRootNodePrivate::doDetachFromParent()
{
    LOG_CRITICAL << "SceneRootNode can't be detached";
}

std::weak_ptr<Scene> &SceneRootNodePrivate::scene()
{
    return m_scene;
}

}
}

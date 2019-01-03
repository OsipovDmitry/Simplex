#include <queue>

#include <scenegraph/scene.h>
#include <scenegraph/abstractsceneoptimizer.h>

#include "scene_p.h"

namespace scenegraph
{

ScenePtr Scene::create(AbstractSceneOptimizerPtr optimizer)
{
    auto scene = ScenePtr(new Scene(optimizer), NodeDeleter());
    scene->m_->scene = scene;
    optimizer->nodeCreated(scene.get());
    optimizer->nodeAttached(scene.get());
    return scene;
}

Scene::Scene(AbstractSceneOptimizerPtr optimizer) :
    Node(new ScenePrivate(optimizer))
{
}

Scene::~Scene()
{
    auto optimizer = static_cast<ScenePrivate*>(m())->optimizer;

    std::queue<Node*> nodes;
    nodes.push(this);

    while (nodes.empty() == false)
    {
        auto node = nodes.front();
        nodes.pop();

        optimizer->nodeDetached(node);

        for (auto child: node->m()->children)
            nodes.push(child.get());

        optimizer->nodeDestroyed(node);
    }
}

}

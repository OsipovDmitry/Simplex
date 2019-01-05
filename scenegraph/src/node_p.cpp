#include <glm/gtc/matrix_transform.hpp>

#include <scenegraph/scene.h>
#include <scenegraph/abstractsceneoptimizer.h>

#include "node_p.h"
#include "scene_p.h"

namespace scenegraph
{

NodePrivate::NodePrivate(Node *pPublicNode, ScenePtr s) :
    pPublic(pPublicNode),
    scene(s),
    needUpdateTransform(true)
{}

void NodePrivate::updateTransform()
{
    needUpdateTransform = true;
    static_cast<ScenePrivate*>(scene.lock()->m())->optimizer->nodeTransformationChanged(pPublic);

    for (auto child: children)
        child->m()->updateTransform();
}

void NodePrivate::recalcTransform()
{
    cacheLocalTransform = glm::translate(glm::mat4x4(), position) * glm::mat4_cast(orientation);
    cacheWorldTransform = (parent.expired() ? glm::mat4x4() : parent.lock()->worldTransform()) * cacheLocalTransform;
    needUpdateTransform = false;
}




}

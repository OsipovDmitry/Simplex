#ifndef SCENEGRAPHFORWARDDECL_H
#define SCENEGRAPHFORWARDDECL_H

#include <memory>
#include <list>

namespace scenegraph
{

class Scene;
using ScenePtr = std::shared_ptr<Scene>;
using SceneWPtr = std::weak_ptr<Scene>;

class AbstractSceneOptimizer;
using AbstractSceneOptimizerPtr = std::shared_ptr<AbstractSceneOptimizer>;

class Node;
using NodePtr = std::shared_ptr<Node>;
using NodeWPtr = std::weak_ptr<Node>;
using NodeList = std::list<NodePtr>;

class Drawable;
using DrawablePtr = std::shared_ptr<Drawable>;

}

#endif // SCENEGRAPHFORWARDDECL_H

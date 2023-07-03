#ifndef CORE_FORWARDDECL_H
#define CORE_FORWARDDECL_H

namespace simplex
{
namespace core
{

class INamedObject;
class IApplication;
class IEngine;
class IGraphicsRenderer;

class SortedObject;

class RenderInfo;

class ApplicationBase;

class GraphicsEngine;

class Viewport;

class IDrawable;
class DrawableBase;
class ColoredDrawable;

class Scene;
class Node;
class SceneRootNode;
class CameraNode;
class DrawableNode;

class NodeVisitor;
class ConditionalNodeVisitor;
class FrustumCullingVisitor;
template <typename NodeClass>
class CollectorVisitor;

}
}

#endif // CORE_FORWARDDECL_H

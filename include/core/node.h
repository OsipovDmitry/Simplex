#ifndef CORE_NODE_H
#define CORE_NODE_H

#include <memory>

#include <utils/forwarddecl.h>
#include <utils/noncopyble.h>
#include <utils/pimpl.h>
#include <utils/treenode.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>
#include <core/inamedobject.h>

namespace simplex
{
namespace core
{

class NodePrivate;
class CORE_SHARED_EXPORT Node : public INamedObject, public utils::TreeNode<Node>
{
    PRIVATE_IMPL(Node)
public:
    Node(const std::string&);
    ~Node() override;

    const std::string &name() const override;

    std::shared_ptr<Node> rootNode();
    std::shared_ptr<const Node> rootNode() const;

    std::shared_ptr<Scene> scene();
    std::shared_ptr<const Scene> scene() const;

    const utils::Transform &globalTransform() const;
    const utils::Transform &transform() const;
    void setTransform(const utils::Transform&);

    const utils::BoundingBox &boundingBox() const;

    void acceptUp(NodeVisitor&);
    void acceptDown(NodeVisitor&);

    virtual std::shared_ptr<Node> asNode();
    virtual std::shared_ptr<const Node> asNode() const;

    virtual std::shared_ptr<SceneRootNode> asSceneRootNode();
    virtual std::shared_ptr<const SceneRootNode> asSceneRootNode() const;

    virtual std::shared_ptr<CameraNode> asCameraNode();
    virtual std::shared_ptr<const CameraNode> asCameraNode() const;

    virtual std::shared_ptr<DrawableNode> asDrawableNode();
    virtual std::shared_ptr<const DrawableNode> asDrawableNode() const;

    virtual std::shared_ptr<LightNode> asLightNode();
    virtual std::shared_ptr<const LightNode> asLightNode() const;

    virtual std::shared_ptr<SoundNode> asSoundNode();
    virtual std::shared_ptr<const SoundNode> asSoundNode() const;

    virtual std::shared_ptr<ListenerNode> asListenerNode();
    virtual std::shared_ptr<const ListenerNode> asListenerNode() const;

protected:
    Node(std::unique_ptr<NodePrivate>);

    void doAttach() override final;
    void doDetach() override final;

    std::unique_ptr<NodePrivate> m_;
};

}
}

#endif // CORE_NODE_H

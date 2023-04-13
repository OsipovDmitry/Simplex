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
class CORE_SHARED_EXPORT Node : public INamedObject, public utils::TreeNode<Node>, public std::enable_shared_from_this<Node>
{
    NONCOPYBLE(Node)
    PIMPL(Node)
public:
    Node(const std::string&);
    ~Node() override;

    const std::string &name() const override;

    virtual std::shared_ptr<Node> asNode();
    virtual std::shared_ptr<const Node> asNode() const;

    virtual std::shared_ptr<SceneRootNode> asSceneRootNode();
    virtual std::shared_ptr<const SceneRootNode> asSceneRootNode() const;

    virtual std::shared_ptr<CameraNode> asCameraNode();
    virtual std::shared_ptr<const CameraNode> asCameraNode() const;

    virtual std::shared_ptr<DrawableNode> asDrawableNode();
    virtual std::shared_ptr<const DrawableNode> asDrawableNode() const;

    const utils::Transform &localTransform() const;
    void setLocalTransform(const utils::Transform&);

    const utils::Transform &globalTransform() const;

    const utils::BoundingBox &minimalBoundingBox() const;
    void setMinimalBoundingBox(const utils::BoundingBox&);

    const utils::BoundingBox &localBoundingBox() const;
    const utils::BoundingBox &globalBoundingBox() const;

    void accept(std::shared_ptr<NodeVisitor>);

protected:
    Node(NodePrivate*);

    std::unique_ptr<NodePrivate> m_;
};

}
}

#endif // CORE_NODE_H

#include <core/nodevisitor.h>
#include <core/node.h>
#include <core/scenerootnode.h>

#include "nodeprivate.h"
#include "nodevisitorhelpers.h"

namespace simplex
{
namespace core
{

Node::Node(const std::string &name)
    : m_(std::make_unique<NodePrivate>(*this, name))
{
}

Node::~Node() = default;

const std::string &Node::name() const
{
    return m_->name();
}

std::shared_ptr<Node> Node::rootNode()
{
    FindRootNodeVisitor findRootNodeVisitor;
    acceptUp(findRootNodeVisitor);
    return findRootNodeVisitor.rootNode();
}

std::shared_ptr<const Node> Node::rootNode() const
{
    return const_cast<Node*>(this)->rootNode();
}

std::shared_ptr<Scene> Node::scene()
{
    std::shared_ptr<Scene> result;
    if (auto sceneRootNode = rootNode()->asSceneRootNode())
        result = sceneRootNode->scene();

    return result;
}

std::shared_ptr<const Scene> Node::scene() const
{
    return const_cast<Node*>(this)->scene();
}

std::shared_ptr<Node> Node::asNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<Node>(wp.lock());
}

std::shared_ptr<const Node> Node::asNode() const
{
    return const_cast<Node*>(this)->asNode();
}

std::shared_ptr<SceneRootNode> Node::asSceneRootNode()
{
    return nullptr;
}

std::shared_ptr<const SceneRootNode> Node::asSceneRootNode() const
{
    return nullptr;
}

std::shared_ptr<CameraNode> Node::asCameraNode()
{
    return nullptr;
}

std::shared_ptr<const CameraNode> Node::asCameraNode() const
{
    return nullptr;
}

std::shared_ptr<DrawableNode> Node::asDrawableNode()
{
    return nullptr;
}

std::shared_ptr<const DrawableNode> Node::asDrawableNode() const
{
    return nullptr;
}

std::shared_ptr<LightNode> Node::asLightNode()
{
    return nullptr;
}

std::shared_ptr<const LightNode> Node::asLightNode() const
{
    return nullptr;
}

std::shared_ptr<SoundNode> Node::asSoundNode()
{
    return nullptr;
}

std::shared_ptr<const SoundNode> Node::asSoundNode() const
{
    return nullptr;
}

std::shared_ptr<ListenerNode> Node::asListenerNode()
{
    return nullptr;
}

std::shared_ptr<const ListenerNode> Node::asListenerNode() const
{
    return nullptr;
}

const utils::Transform &Node::globalTransform() const
{
    return m_->globalTransform();
}

const utils::Transform &Node::transform() const
{
    return m_->transform();
}

void Node::setTransform(const utils::Transform &t)
{
    BeforeTransformChangedNodeVisitor beforeTransformChangedNodeVisitor;
    acceptDown(beforeTransformChangedNodeVisitor);

    m_->transform() = t;

    m_->dirtyGlobalTransform();

    AfterTransformChangedNodeVisitor afterTransformChangedNodeVisitor;
    acceptDown(afterTransformChangedNodeVisitor);
}

void Node::acceptUp(NodeVisitor &nodeVisitor)
{
    if (nodeVisitor.visit(*this))
        if (auto p = parent())
            p->acceptUp(nodeVisitor);
}

void Node::acceptDown(NodeVisitor &nodeVisitor)
{
    if (nodeVisitor.visit(*this))
        for (auto &child : children())
            child->acceptDown(nodeVisitor);
}

Node::Node(std::unique_ptr<NodePrivate> nodePrivate)
    : m_(std::move(nodePrivate))
{
}

void Node::doAttach()
{
    m_->dirtyGlobalTransform();

    m_->doAttachToParent(parent());

    if (auto s = scene())
    {
        AttachToSceneNodeVisitor attachToSceneNodeVisitor(s);
        acceptDown(attachToSceneNodeVisitor);
    }
}

void Node::doDetach()
{
    if (auto s = scene())
    {
        DetachFromSceneNodeVisitor detachFromSceneNodeVisitor(s);
        acceptDown(detachFromSceneNodeVisitor);
    }

    m_->doDetachFromParent(parent());

    m_->dirtyGlobalTransform();
}

}
}

#include <core/nodevisitor.h>
#include <core/node.h>
#include <core/scenerootnode.h>

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

Node::Node(const std::string &name)
    : m_(std::make_unique<NodePrivate>(name))
{
}

Node::~Node()
{
}

const std::string &Node::name() const
{
    return m_->name();
}

std::shared_ptr<const Scene> Node::scene() const
{
    return const_cast<Node*>(this)->scene();
}

std::shared_ptr<Scene> Node::scene()
{
    auto p = asNode();
    for (auto parentNode = p->parent(); parentNode; parentNode = parentNode->parent())
        p = parentNode;

    std::shared_ptr<Scene> result = nullptr;
    if (auto sceneRootNode = p->asSceneRootNode(); sceneRootNode)
        result = sceneRootNode->scene();

    return result;
}

std::shared_ptr<Node> Node::asNode()
{
    return shared_from_this();
}

std::shared_ptr<const Node> Node::asNode() const
{
    return shared_from_this();
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

const utils::Transform &Node::transform() const
{
    return m_->transform();
}

void Node::setTransform(const utils::Transform &t)
{
    m_->transform() = t;
    NodePrivate::dirtyGlobalTransform(asNode());
    NodePrivate::dirtyBoundingBox(asNode());
}

const utils::Transform &Node::globalTransform() const
{
    if (m_->isGlobalTransformDirty())
    {
        m_->globalTransform() = m_->transform();
        if (auto parentNode = parent(); parentNode)
            m_->globalTransform() = parentNode->globalTransform() * m_->globalTransform();
        m_->isGlobalTransformDirty() = false;
    }
    return m_->globalTransform();
}

const utils::BoundingBox &Node::boundingBox() const
{
    if (m_->isBoundingBoxDirty())
    {
        auto &bb = m_->globalBoundingBox();
        bb = utils::BoundingBox();
        for (auto &child : children())
            bb += child->transform() * child->boundingBox();
        m_->isBoundingBoxDirty() = false;
    }

    return m_->globalBoundingBox();
}

void Node::accept(NodeVisitor &nodeVisitor)
{
    if (nodeVisitor.visit(asNode()))
        for (auto &child : children())
            child->accept(nodeVisitor);
}

Node::Node(std::unique_ptr<NodePrivate> nodePrivate)
    : m_(std::move(nodePrivate))
{
}

void Node::doAttach()
{
    NodePrivate::dirtyGlobalTransform(asNode());
    if (auto p = parent(); p)
        NodePrivate::dirtyBoundingBox(p);
}

void Node::doDetach()
{
    NodePrivate::dirtyGlobalTransform(asNode());
    if (auto p = parent(); p)
        NodePrivate::dirtyBoundingBox(p);
}

}
}

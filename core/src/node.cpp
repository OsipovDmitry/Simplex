#include <utils/logger.h>

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
    : m_(std::make_unique<NodePrivate>(name))
{
}

Node::~Node() = default;

const std::string &Node::name() const
{
    return m_->name();
}

std::shared_ptr<const Node> Node::rootNode() const
{
    return const_cast<Node*>(this)->rootNode();
}

std::shared_ptr<Node> Node::rootNode()
{
    FindRootNodeVisitor findRootNodeVisitor;
    acceptUp(findRootNodeVisitor);
    return findRootNodeVisitor.rootNode();
}

std::shared_ptr<const Scene> Node::scene() const
{
    return const_cast<Node*>(this)->scene();
}

std::shared_ptr<Scene> Node::scene()
{
    std::shared_ptr<Scene> result;
    if (auto sceneNode = rootNode()->asSceneRootNode(); sceneNode)
        result = sceneNode->scene();

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

const utils::Transform &Node::globalTransform() const
{
    if (m_->isGlobalTransformDirty())
    {
        utils::Transform t = m_->transform();
        if (auto parentNode = parent(); parentNode)
            t = parentNode->globalTransform() * t;

        m_->globalTransform() = t;
        m_->isGlobalTransformDirty() = false;
    }

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

    dirtyGlobalTransform();
    dirtyBoundingBox();

    AfterTransformChangedNodeVisitor afterTransformChangedNodeVisitor;
    acceptDown(afterTransformChangedNodeVisitor);
}

const utils::BoundingBox &Node::boundingBox() const
{
    static const auto calculateBoundingBox = [](const std::shared_ptr<const Node> &node)
    {
        static const auto calculateStandardBoundingBoxImpl = [](const auto& impl,
                                                                const std::shared_ptr<const Node> &node) -> const utils::BoundingBox&
        {
            utils::BoundingBox bb;

            if (node->boundingBoxPolicy() == BoundingBoxPolicy::Standard)
                bb += node->doBoundingBox();

            for (const auto &child : node->children())
                bb += child->transform() * impl(impl, child);

            if (auto sceneRootNode = node->asSceneRootNode(); sceneRootNode)
                bb = utils::Transform::fromScale(1.05f) * bb;

            auto &nodePrivate = node->m();
            nodePrivate.boundingBox() = bb;
            nodePrivate.isBoundingBoxDirty() = false;

            return nodePrivate.boundingBox();
        };

        static const auto calculateRootBoundingBoxImpl = [](const auto& impl,
                                                            const std::shared_ptr<const Node> &node,
                                                            const std::shared_ptr<const Node> &rootNode) -> void
        {
            if (node->boundingBoxPolicy() == BoundingBoxPolicy::Root)
            {
                const auto rootNode2NodeTransform = rootNode->globalTransform() * node->globalTransform().inverted();
                node->m().boundingBox() = rootNode2NodeTransform * rootNode->m().boundingBox();
            }

            for (const auto &child : node->children())
                impl(impl, child, rootNode);
        };

        calculateStandardBoundingBoxImpl(calculateStandardBoundingBoxImpl, node);
        calculateRootBoundingBoxImpl(calculateRootBoundingBoxImpl, node, node);
    };

    if (m_->isBoundingBoxDirty())
    {
        std::shared_ptr<const Node> targetNode;
        switch (m_->boundingBoxPolicy())
        {
        case BoundingBoxPolicy::Standard:
            targetNode = asNode();
            break;
        case BoundingBoxPolicy::Root:
            targetNode = rootNode();
            break;
        default:
            LOG_CRITICAL << "Undefined bounding box policy";
            break;
        }

        calculateBoundingBox(targetNode);
    }

    return m_->boundingBox();
}

BoundingBoxPolicy Node::boundingBoxPolicy() const
{
    return m_->boundingBoxPolicy();
}

void Node::setBoundingBoxPolicy(BoundingBoxPolicy value)
{
    m_->boundingBoxPolicy() = value;
    dirtyBoundingBox();
}

void Node::acceptUp(NodeVisitor &nodeVisitor)
{
    if (nodeVisitor.visit(asNode()))
        if (auto p = parent(); p)
            p->acceptUp(nodeVisitor);
}

void Node::acceptDown(NodeVisitor &nodeVisitor)
{
    if (nodeVisitor.visit(asNode()))
        for (auto &child : children())
            child->acceptDown(nodeVisitor);
}

void Node::doUpdate(uint64_t, uint32_t)
{
}

utils::BoundingBox Node::doBoundingBox() const
{
    return utils::BoundingBox();
}

void Node::doBeforeTransformChanged()
{
}

void Node::doAfterTransformChanged()
{
}

Node::Node(std::unique_ptr<NodePrivate> nodePrivate)
    : m_(std::move(nodePrivate))
{
}

void Node::doAttach()
{
    dirtyGlobalTransform();
    dirtyBoundingBox();
}

void Node::doDetach()
{
    dirtyGlobalTransform();
    dirtyBoundingBox();
}

void Node::dirtyGlobalTransform()
{
    DirtyGlobalTransformNodeVisitor dirtyGlobalTransformNodeVisitor;
    acceptDown(dirtyGlobalTransformNodeVisitor);
}

void Node::dirtyBoundingBox()
{
    DirtyBoundingBoxNodeVisitor dirtyStandardBoundingBoxNodeVisitor({BoundingBoxPolicy::Standard});
    acceptUp(dirtyStandardBoundingBoxNodeVisitor);

    DirtyBoundingBoxNodeVisitor dirtyRootBoundingBoxNodeVisitor({BoundingBoxPolicy::Root});
    acceptDown(dirtyRootBoundingBoxNodeVisitor);
}

}
}

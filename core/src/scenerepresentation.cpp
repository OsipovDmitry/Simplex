#include <core/cameranode.h>
#include <core/drawablenode.h>
#include <core/scenerepresentation.h>
#include <core/skeletalanimatednode.h>

#include "scenerepresentationprivate.h"

namespace simplex
{
namespace core
{

NodeRepresentation::NodeRepresentation(
    const std::string& name,
    const utils::Transform& transform,
    const std::vector<size_t>& children)
    : NodeRepresentation(std::make_unique<NodeRepresentationPrivate>(name, transform, children))
{
}

NodeRepresentation::~NodeRepresentation() = default;

const std::string& NodeRepresentation::name() const
{
    return m_->name();
}

std::shared_ptr<const NodeRepresentation> NodeRepresentation::asNodeRepresentation() const
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<const NodeRepresentation>(wp.lock());
}

std::shared_ptr<const DrawableNodeRepresentation> NodeRepresentation::asDrawableNodeRepresentation() const
{
    return nullptr;
}

std::shared_ptr<const CameraNodeRepresentation> NodeRepresentation::asCameraNodeRepresentation() const
{
    return nullptr;
}

std::shared_ptr<const LightNodeRepresentation> NodeRepresentation::asLightNodeRepresentation() const
{
    return nullptr;
}

NodeRepresentation::NodeRepresentation(std::unique_ptr<NodeRepresentationPrivate>&& nodeRepresentationPrivate)
    : m_(std::move(nodeRepresentationPrivate))
{
}

DrawableNodeRepresentation::DrawableNodeRepresentation(
    const std::string& name,
    const utils::Transform& transform,
    const std::vector<size_t>& children,
    const std::vector<size_t>& drawables)
    : NodeRepresentation(std::make_unique<DrawableNodeRepresentationPrivate>(name, transform, children, drawables))
{
}

DrawableNodeRepresentation::~DrawableNodeRepresentation() = default;

std::shared_ptr<const DrawableNodeRepresentation> DrawableNodeRepresentation::asDrawableNodeRepresentation() const
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<const DrawableNodeRepresentation>(wp.lock());
}

CameraNodeRepresentation::CameraNodeRepresentation(
    const std::string& name,
    const utils::Transform& transform,
    const std::vector<size_t>& children,
    const utils::ClipSpace& clipspace,
    const utils::Range& cullPlaneLimits)
    : NodeRepresentation(std::make_unique<CameraNodeRepresentationPrivate>(name, transform, children, clipspace, cullPlaneLimits))
{
}

CameraNodeRepresentation::~CameraNodeRepresentation() = default;

std::shared_ptr<const CameraNodeRepresentation> CameraNodeRepresentation::asCameraNodeRepresentation() const
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<const CameraNodeRepresentation>(wp.lock());
}

SceneRepresentation::SceneRepresentation(
    const std::string& name,
    const std::vector<std::shared_ptr<Drawable>>& drawables,
    const std::vector<std::shared_ptr<NodeRepresentation>>& nodesRepresentations,
    const std::vector<std::shared_ptr<Animation>>& animations,
    size_t rootNodeID)
    : m_(std::make_unique<SceneRepresentationPrivate>(name, drawables, nodesRepresentations, animations, rootNodeID))
{
}

SceneRepresentation::~SceneRepresentation() = default;

const std::string& SceneRepresentation::name() const
{
    return m_->name();
}

std::shared_ptr<SkeletalAnimatedNode> SceneRepresentation::generateAnimatedNode(
    const std::string& name,
    bool insertDrawables,
    bool insertAnimations,
    bool insertCameras,
    bool insertLights) const
{
    static auto createNode = [](
        auto& self,
        size_t nodeRepresentationID,
        const std::vector<std::shared_ptr<Drawable>>& drawables,
        const std::vector<std::shared_ptr<NodeRepresentation>>& nodesRepresentations) -> std::shared_ptr<Node>
        {
            std::shared_ptr<Node> node;

            const auto& nodeRepresentation = nodesRepresentations[nodeRepresentationID];
            const auto& nodeRepresentationName = nodeRepresentation->name();

            if (auto drawableNodeRepresentation = nodeRepresentation->asDrawableNodeRepresentation())
            {
                auto drawableNode = std::make_shared<DrawableNode>(nodeRepresentationName);

                auto& drawableNodeRepresentationPrivate = drawableNodeRepresentation->m();
                for (auto drawableID : drawableNodeRepresentationPrivate.drawablesIDs())
                    drawableNode->addDrawable(drawables[drawableID]);

                node = drawableNode;
            }
            else
            {
                node = std::make_shared<Node>(nodeRepresentationName);
            }

            auto& nodeRepresentationPrivate = nodeRepresentation->m();

            node->setTransform(nodeRepresentationPrivate.transform());

            for (auto nodeRepresentationChildID : nodeRepresentationPrivate.childrenIDs())
                node->attach(self(
                    self,
                    nodeRepresentationChildID,
                    drawables,
                    nodesRepresentations));

            return node;
        };

    auto result = std::make_shared<SkeletalAnimatedNode>(name);

    result->attach(createNode(
        createNode,
        m_->rootNodeID(),
        m_->drawables(),
        m_->nodesRepresentations()));

    for (const auto& animation : m_->animations())
        result->addAnimation(animation);

    return result;
}

}
}

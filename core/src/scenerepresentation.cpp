#include <core/cameranode.h>
#include <core/drawablenode.h>
#include <core/lightnode.h>
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

std::shared_ptr<const BoneNodeRepresentation> NodeRepresentation::asBoneNodeRepresentation() const
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

BoneNodeRepresentation::BoneNodeRepresentation(
    const std::string& name,
    const utils::Transform& transform,
    const std::vector<size_t>& childrenIDs,
    uint32_t boneID)
    : NodeRepresentation(std::make_unique<BoneNodeRepresentationPrivate>(
        name,
        transform,
        childrenIDs,
        boneID))
{
}

BoneNodeRepresentation::~BoneNodeRepresentation() = default;

std::shared_ptr<const BoneNodeRepresentation> BoneNodeRepresentation::asBoneNodeRepresentation() const
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<const BoneNodeRepresentation>(wp.lock());
}

std::shared_ptr<const RootBoneNodeRepresentation> BoneNodeRepresentation::asRootBoneNodeRepresentation() const
{
    return nullptr;
}

BoneNodeRepresentation::BoneNodeRepresentation(std::unique_ptr<BoneNodeRepresentationPrivate>&& boneNodeRepresentationPrivate)
    : NodeRepresentation(std::move(boneNodeRepresentationPrivate))
{
}

RootBoneNodeRepresentation::RootBoneNodeRepresentation(
    const std::string& name,
    const utils::Transform& transform,
    const std::vector<size_t>& childrenIDs,
    uint32_t boneID,
    uint32_t skeletonID)
    : BoneNodeRepresentation(std::make_unique<RootBoneNodeRepresentationPrivate>(
        name,
        transform,
        childrenIDs,
        boneID,
        skeletonID))
{
}

RootBoneNodeRepresentation::~RootBoneNodeRepresentation() = default;

std::shared_ptr<const RootBoneNodeRepresentation> RootBoneNodeRepresentation::asRootBoneNodeRepresentation() const
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<const RootBoneNodeRepresentation>(wp.lock());
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

LightNodeRepresentation::LightNodeRepresentation(
    const std::string& name,
    const utils::Transform& transform,
    const std::vector<size_t>& children)
    : NodeRepresentation(std::make_unique<LightNodeRepresentationPrivate>(name, transform, children))
{
}

LightNodeRepresentation::~LightNodeRepresentation() = default;

std::shared_ptr<const LightNodeRepresentation> LightNodeRepresentation::asLightNodeRepresentation() const
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<const LightNodeRepresentation>(wp.lock());
}

SceneRepresentation::SceneRepresentation(
    const std::string& name,
    const std::vector<std::shared_ptr<Drawable>>& drawables,
    const std::vector<std::shared_ptr<Skeleton>>& skeletons,
    const std::vector<std::shared_ptr<NodeRepresentation>>& nodesRepresentations,
    size_t rootNodeID)
    : m_(std::make_unique<SceneRepresentationPrivate>(name, drawables, skeletons, nodesRepresentations, rootNodeID))
{
}

SceneRepresentation::~SceneRepresentation() = default;

const std::string& SceneRepresentation::name() const
{
    return m_->name();
}

std::shared_ptr<Node> SceneRepresentation::generate(
    const std::string& name,
    bool insertCameras,
    bool insertLights) const
{
    static auto createNode = [](
        auto& self,
        size_t nodeRepresentationID,
        const std::vector<std::shared_ptr<Drawable>>& drawables,
        const std::vector<std::shared_ptr<Skeleton>>& skeletons,
        const std::vector<std::shared_ptr<NodeRepresentation>>& nodesRepresentations,
        bool insertCameras,
        bool insertLights) -> std::shared_ptr<Node>
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
            else if (auto boneNodeRepresentation = nodeRepresentation->asBoneNodeRepresentation())
            {
                if (auto rootBoneNodeRepresentation = boneNodeRepresentation->asRootBoneNodeRepresentation())
                {
                    auto& rootBoneNodeRepresentationPrivate = rootBoneNodeRepresentation->m();

                    auto skeletalAnimatedNode = std::make_shared<SkeletalAnimatedNode>(
                        nodeRepresentationName,
                        rootBoneNodeRepresentationPrivate.boneID(),
                        skeletons[rootBoneNodeRepresentationPrivate.skeletonID()]);

                    node = skeletalAnimatedNode;
                }
                else
                {
                    auto& boneNodeRepresentationPrivate = boneNodeRepresentation->m();

                    auto boneNode = std::make_shared<BoneNode>(
                        nodeRepresentationName,
                        boneNodeRepresentationPrivate.boneID());

                    node = boneNode;
                }
            }
            else if (auto cameraNodeRepresentation = nodeRepresentation->asCameraNodeRepresentation();
                cameraNodeRepresentation && insertCameras)
            {
                auto cameraNode = std::make_shared<CameraNode>(nodeRepresentationName);

                auto& cameraNodeRepresentationPrivate = cameraNodeRepresentation->m();
                //

                node = cameraNode;
            }
            else if (auto lightNodeRepresentation = nodeRepresentation->asLightNodeRepresentation();
                lightNodeRepresentation && insertLights)
            {
                std::shared_ptr<LightNode> lightNode;

                auto& lightNodeRepresentationPrivate = lightNodeRepresentation->m();
                //

                node = lightNode;
            }
            else
            {
                node = std::make_shared<Node>(nodeRepresentationName);
            }

            auto& nodeRepresentationPrivate = nodeRepresentation->m();

            node->setTransform(nodeRepresentationPrivate.transform());

            for (auto nodeRepresentationChildID : nodeRepresentationPrivate.childrenIDs())
            {
                node->attach(self(
                    self,
                    nodeRepresentationChildID,
                    drawables,
                    skeletons,
                    nodesRepresentations,
                    insertCameras,
                    insertLights));
            }

            return node;
        };

    auto result = std::make_shared<Node>(name);

    result->attach(createNode(
        createNode,
        m_->rootNodeID(),
        m_->drawables(),
        m_->skeletons(),
        m_->nodesRepresentations(),
        insertCameras,
        insertLights));

    return result;
}

}
}

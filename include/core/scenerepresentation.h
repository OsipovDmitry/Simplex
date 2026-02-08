#ifndef CORE_SCENEREPRESENTATION_H
#define CORE_SCENEREPRESENTATION_H

#include <utils/forwarddecl.h>
#include <utils/noncopyble.h>
#include <utils/pimpl.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>
#include <core/inamedobject.h>

namespace simplex
{
namespace core
{

class NodeRepresentationPrivate;
class CORE_SHARED_EXPORT NodeRepresentation : public INamedObject, public std::enable_shared_from_this<NodeRepresentation>
{
    NONCOPYBLE(NodeRepresentation)
    PRIVATE_IMPL(NodeRepresentation)
public:
    NodeRepresentation(
        const std::string& name,
        const utils::Transform& transform,
        const std::vector<size_t>& childrenIDs);
    ~NodeRepresentation() override;

    const std::string& name() const override;

    virtual std::shared_ptr<const NodeRepresentation> asNodeRepresentation() const;
    virtual std::shared_ptr<const DrawableNodeRepresentation> asDrawableNodeRepresentation() const;
    virtual std::shared_ptr<const BoneNodeRepresentation> asBoneNodeRepresentation() const;
    virtual std::shared_ptr<const CameraNodeRepresentation> asCameraNodeRepresentation() const;
    virtual std::shared_ptr<const LightNodeRepresentation> asLightNodeRepresentation() const;

protected:
    NodeRepresentation(std::unique_ptr<NodeRepresentationPrivate>&&);

    std::unique_ptr<NodeRepresentationPrivate> m_;
};

class DrawableNodeRepresentationPrivate;
class CORE_SHARED_EXPORT DrawableNodeRepresentation : public NodeRepresentation
{
    PRIVATE_IMPL(DrawableNodeRepresentation)
public:
    DrawableNodeRepresentation(
        const std::string& name,
        const utils::Transform& transform,
        const std::vector<size_t>& childrenIDs,
        const std::vector<size_t>& drawablesIDs);
    ~DrawableNodeRepresentation() override;

    std::shared_ptr<const DrawableNodeRepresentation> asDrawableNodeRepresentation() const override;
};

class BoneNodeRepresentationPrivate;
class CORE_SHARED_EXPORT BoneNodeRepresentation : public NodeRepresentation
{
    PRIVATE_IMPL(BoneNodeRepresentation)
public:
    BoneNodeRepresentation(
        const std::string& name,
        const utils::Transform& transform,
        const std::vector<size_t>& childrenIDs,
        uint32_t boneID);
    ~BoneNodeRepresentation() override;

    std::shared_ptr<const BoneNodeRepresentation> asBoneNodeRepresentation() const override;

    virtual std::shared_ptr<const RootBoneNodeRepresentation> asRootBoneNodeRepresentation() const;

protected:
    BoneNodeRepresentation(std::unique_ptr<BoneNodeRepresentationPrivate>&&);
};

class RootBoneNodeRepresentationPrivate;
class CORE_SHARED_EXPORT RootBoneNodeRepresentation : public BoneNodeRepresentation
{
    PRIVATE_IMPL(RootBoneNodeRepresentation)
public:
    RootBoneNodeRepresentation(
        const std::string& name,
        const utils::Transform& transform,
        const std::vector<size_t>& childrenIDs,
        uint32_t skeletonID,
        uint32_t boneID);
    ~RootBoneNodeRepresentation() override;

    std::shared_ptr<const RootBoneNodeRepresentation> asRootBoneNodeRepresentation() const override;
};

class CameraNodeRepresentationPrivate;
class CORE_SHARED_EXPORT CameraNodeRepresentation : public NodeRepresentation
{
    PRIVATE_IMPL(CameraNodeRepresentation)
public:
    CameraNodeRepresentation(
        const std::string& name,
        const utils::Transform& transform,
        const std::vector<size_t>& childrenIDs,
        const utils::ClipSpace& clipspace,
        const utils::Range& cullPlaneLimits);
    ~CameraNodeRepresentation() override;

    std::shared_ptr<const CameraNodeRepresentation> asCameraNodeRepresentation() const override;
};

class LightNodeRepresentationPrivate;
class CORE_SHARED_EXPORT LightNodeRepresentation : public NodeRepresentation
{
    PRIVATE_IMPL(LightNodeRepresentation)
public:
    LightNodeRepresentation(
        const std::string& name,
        const utils::Transform& transform,
        const std::vector<size_t>& childrenIDs);
    ~LightNodeRepresentation() override;

    std::shared_ptr<const LightNodeRepresentation> asLightNodeRepresentation() const override;
};

class SceneRepresentationPrivate;
class CORE_SHARED_EXPORT SceneRepresentation final : public INamedObject 
{
    NONCOPYBLE(SceneRepresentation)
public:
    SceneRepresentation(
        const std::string& name,
        const std::vector<std::shared_ptr<Drawable>>& drawables,
        const std::vector<std::shared_ptr<Skeleton>>& skeletons,
        const std::vector<std::shared_ptr<NodeRepresentation>>& nodesRepresentations,
        size_t rootNodeID);
    ~SceneRepresentation() override;

    const std::string& name() const override;

    std::shared_ptr<Node> generate(
        const std::string& name,
        bool insertCameras,
        bool insertLights) const;

private:
    std::unique_ptr<SceneRepresentationPrivate> m_;

};

}
}

#endif // CORE_SCENEREPRESENTATION_H

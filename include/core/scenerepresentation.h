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

    virtual std::shared_ptr<const DrawableNodeRepresentation> asDrawableNodeRepresentation() const;
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

    virtual std::shared_ptr<const CameraNodeRepresentation> asCameraNodeRepresentation() const;
};

class SceneRepresentationPrivate;
class CORE_SHARED_EXPORT SceneRepresentation final : public INamedObject 
{
    NONCOPYBLE(SceneRepresentation)
public:
    SceneRepresentation(
        const std::string& name,
        const std::vector<std::shared_ptr<Drawable>>& drawables,
        const std::vector<std::shared_ptr<NodeRepresentation>>& nodesRepresentations,
        const std::vector<std::shared_ptr<Animation>>& animations,
        size_t rootNodeID);
    ~SceneRepresentation() override;

    const std::string& name() const override;

    std::shared_ptr<SkeletalAnimatedNode> generateAnimatedNode(
        const std::string& name,
        bool insertDrawables,
        bool insertAnimations,
        bool insertCameras,
        bool insertLights) const;

private:
    std::unique_ptr<SceneRepresentationPrivate> m_;

};

}
}

#endif // CORE_SCENEREPRESENTATION_H

#ifndef CORE_SCENEREPRESENTATIONEPRIVATE_H
#define CORE_SCENEREPRESENTATIONEPRIVATE_H

#include <string>
#include <vector>

#include <utils/clipspace.h>
#include <utils/range.h>
#include <utils/transform.h>

namespace simplex
{
namespace core
{

class NodeRepresentationPrivate
{
public:
    NodeRepresentationPrivate(const std::string&, const utils::Transform&, const std::vector<size_t>&);
    virtual ~NodeRepresentationPrivate();

    std::string& name();
    utils::Transform& transform();
    std::vector<size_t>& childrenIDs();

private:
    std::string m_name;
    utils::Transform m_transform;
    std::vector<size_t> m_childrenIDs;
};

class DrawableNodeRepresentationPrivate : public NodeRepresentationPrivate
{
public:
    DrawableNodeRepresentationPrivate(
        const std::string&,
        const utils::Transform&,
        const std::vector<size_t>&,
        const std::vector<size_t>&);
    ~DrawableNodeRepresentationPrivate() override;

    const std::vector<size_t>& drawablesIDs();

private:
    std::vector<size_t> m_drawablesIDs;
};

class BoneNodeRepresentationPrivate : public NodeRepresentationPrivate
{
public:
    BoneNodeRepresentationPrivate(
        const std::string&,
        const utils::Transform&,
        const std::vector<size_t>&,
        uint32_t);
    ~BoneNodeRepresentationPrivate() override;

    uint32_t& boneID();

private:
    uint32_t m_boneID;
};

class RootBoneNodeRepresentationPrivate : public BoneNodeRepresentationPrivate
{
public:
    RootBoneNodeRepresentationPrivate(
        const std::string&,
        const utils::Transform&,
        const std::vector<size_t>&,
        uint32_t,
        uint32_t);
    ~RootBoneNodeRepresentationPrivate() override;

    uint32_t& skeletonID();

private:
    uint32_t m_skeletonID;
};

class CameraNodeRepresentationPrivate : public NodeRepresentationPrivate
{
public:
    CameraNodeRepresentationPrivate(
        const std::string&,
        const utils::Transform&,
        const std::vector<size_t>&,
        const utils::ClipSpace&,
        const utils::Range&);
    ~CameraNodeRepresentationPrivate() override;

    utils::ClipSpace& clipSpace();
    utils::Range& cullPlaneLimits();

private:
    utils::ClipSpace m_clipSpace;
    utils::Range m_cullPlaneLimits;
};

class LightNodeRepresentationPrivate : public NodeRepresentationPrivate
{
public:
    LightNodeRepresentationPrivate(
        const std::string&,
        const utils::Transform&,
        const std::vector<size_t>&);
    ~LightNodeRepresentationPrivate() override;

private:
};

class SceneRepresentationPrivate
{
public:
    SceneRepresentationPrivate(
        const std::string&,
        const std::vector<std::shared_ptr<Drawable>>&,
        const std::vector<std::shared_ptr<Skeleton>>&,
        const std::vector<std::shared_ptr<NodeRepresentation>>&,
        size_t);
    ~SceneRepresentationPrivate();

    std::string& name();
    std::vector<std::shared_ptr<Drawable>>& drawables();
    std::vector<std::shared_ptr<Skeleton>>& skeletons();
    std::vector<std::shared_ptr<NodeRepresentation>>& nodesRepresentations();
    size_t& rootNodeID();

private:
    std::string m_name;
    std::vector<std::shared_ptr<Drawable>> m_drawables;
    std::vector<std::shared_ptr<Skeleton>> m_skeletons;
    std::vector<std::shared_ptr<NodeRepresentation>> m_nodesRepresentations;
    size_t m_rootNodeID;
};


}
}

#endif // CORE_SCENEREPRESENTATIONEPRIVATE_H

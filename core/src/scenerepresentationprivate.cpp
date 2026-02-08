#include <core/scenerepresentation.h>

#include "scenerepresentationprivate.h"

namespace simplex
{
namespace core
{

NodeRepresentationPrivate::NodeRepresentationPrivate(
    const std::string& name,
    const utils::Transform& transform,
    const std::vector<size_t>& childrenIDs)
    : m_name(name)
    , m_transform(transform)
    , m_childrenIDs(childrenIDs)
{
}

NodeRepresentationPrivate::~NodeRepresentationPrivate() = default;

std::string& NodeRepresentationPrivate::name()
{
    return m_name;
}

utils::Transform& NodeRepresentationPrivate::transform()
{
    return m_transform;
}

std::vector<size_t>& NodeRepresentationPrivate::childrenIDs()
{
    return m_childrenIDs;
}

DrawableNodeRepresentationPrivate::DrawableNodeRepresentationPrivate(
    const std::string& name,
    const utils::Transform& transform,
    const std::vector<size_t>& childrenIDs,
    const std::vector<size_t>& drawablesIDs)
    : NodeRepresentationPrivate(name, transform, childrenIDs)
    , m_drawablesIDs(drawablesIDs)
{
}

DrawableNodeRepresentationPrivate::~DrawableNodeRepresentationPrivate() = default;

const std::vector<size_t>& DrawableNodeRepresentationPrivate::drawablesIDs()
{
    return m_drawablesIDs;
}

BoneNodeRepresentationPrivate::BoneNodeRepresentationPrivate(
    const std::string& name,
    const utils::Transform& transform,
    const std::vector<size_t>& childrenIDs,
    uint32_t boneID)
    : NodeRepresentationPrivate(name, transform, childrenIDs)
    , m_boneID(boneID)
{
}

BoneNodeRepresentationPrivate::~BoneNodeRepresentationPrivate() = default;

uint32_t& BoneNodeRepresentationPrivate::boneID()
{
    return m_boneID;
}

RootBoneNodeRepresentationPrivate::RootBoneNodeRepresentationPrivate(
    const std::string& name,
    const utils::Transform& transform,
    const std::vector<size_t>& childrenIDs,
    uint32_t boneID,
    uint32_t skeletonID)
    : BoneNodeRepresentationPrivate(name, transform, childrenIDs, boneID)
    , m_skeletonID(skeletonID)
{
}

RootBoneNodeRepresentationPrivate::~RootBoneNodeRepresentationPrivate() = default;

uint32_t& RootBoneNodeRepresentationPrivate::skeletonID()
{
    return m_skeletonID;
}

CameraNodeRepresentationPrivate::CameraNodeRepresentationPrivate(
    const std::string& name,
    const utils::Transform& transform,
    const std::vector<size_t>& childrenIDs,
    const utils::ClipSpace& clipSpace,
    const utils::Range& cullPlaneLimits)
    : NodeRepresentationPrivate(name, transform, childrenIDs)
    , m_clipSpace(clipSpace)
    , m_cullPlaneLimits(cullPlaneLimits)
{
}

CameraNodeRepresentationPrivate::~CameraNodeRepresentationPrivate() = default;

utils::ClipSpace& CameraNodeRepresentationPrivate::clipSpace()
{
    return m_clipSpace;
}

utils::Range& CameraNodeRepresentationPrivate::cullPlaneLimits()
{
    return m_cullPlaneLimits;
}

LightNodeRepresentationPrivate::LightNodeRepresentationPrivate(
    const std::string& name,
    const utils::Transform& transform,
    const std::vector<size_t>& childrenIDs)
    : NodeRepresentationPrivate(name, transform, childrenIDs)
{
}

LightNodeRepresentationPrivate::~LightNodeRepresentationPrivate() = default;

SceneRepresentationPrivate::SceneRepresentationPrivate(
    const std::string& name,
    const std::vector<std::shared_ptr<Drawable>>& drawables,
    const std::vector<std::shared_ptr<Skeleton>>& skeletons,
    const std::vector<std::shared_ptr<NodeRepresentation>>& nodesRepresentations,
    size_t rootNodeID)
    : m_name(name)
    , m_drawables(drawables)
    , m_skeletons(skeletons)
    , m_nodesRepresentations(nodesRepresentations)
    , m_rootNodeID(rootNodeID)
{
}

SceneRepresentationPrivate::~SceneRepresentationPrivate() = default;

std::string& SceneRepresentationPrivate::name()
{
    return m_name;
}

std::vector<std::shared_ptr<Drawable>>& SceneRepresentationPrivate::drawables()
{
    return m_drawables;
}

std::vector<std::shared_ptr<Skeleton>>& SceneRepresentationPrivate::skeletons()
{
    return m_skeletons;
}

std::vector<std::shared_ptr<NodeRepresentation>>& SceneRepresentationPrivate::nodesRepresentations()
{
    return m_nodesRepresentations;
}

size_t& SceneRepresentationPrivate::rootNodeID()
{
    return m_rootNodeID;
}

}
}

#include <utils/logger.h>

#include <core/lightnode.h>

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

LightNode::~LightNode() = default;

std::shared_ptr<LightNode> LightNode::asLightNode()
{
    return std::dynamic_pointer_cast<LightNode>(shared_from_this());
}

std::shared_ptr<const LightNode> LightNode::asLightNode() const
{
    return std::dynamic_pointer_cast<const LightNode>(shared_from_this());
}

utils::BoundingBox LightNode::doBoundingBox() const
{
    return areaBoundingBox();
}

std::shared_ptr<PointLightNode> LightNode::asPointLightNode()
{
    return nullptr;
}

std::shared_ptr<const PointLightNode> LightNode::asPointLightNode() const
{
    return nullptr;
}

std::shared_ptr<SpotLightNode> LightNode::asSpotLightNode()
{
    return nullptr;
}

std::shared_ptr<const SpotLightNode> LightNode::asSpotLightNode() const
{
    return nullptr;
}

std::shared_ptr<DirectionalLightNode> LightNode::asDirectionalLightNode()
{
    return nullptr;
}

std::shared_ptr<const DirectionalLightNode> LightNode::asDirectionalLightNode() const
{
    return nullptr;
}

std::shared_ptr<IBLLightNode> LightNode::asIBLLightNode()
{
    return nullptr;
}

std::shared_ptr<const IBLLightNode> LightNode::asIBLLightNode() const
{
    return nullptr;
}

bool LightNode::isLightingEnabled() const
{
    return m().isLightingEnabled();
}

void LightNode::setLightingEnabled(bool value)
{
    m().isLightingEnabled() = value;
}

bool LightNode::isShadingEnabled() const
{
    return m().isShadingEnabled();
}

void LightNode::setShadingEnabled(bool value)
{
    m().isShadingEnabled() = value;
}

const glm::mat4x4 &LightNode::areaMatrix() const
{
    auto &mPrivate = m();
    if (mPrivate.isAreaMatrixDirty())
    {
        mPrivate.areaMatrix() = doAreaMatrix();
        mPrivate.isAreaMatrixDirty() = false;
    }
    return mPrivate.areaMatrix();
}

std::shared_ptr<const LightDrawable> LightNode::areaDrawable() const
{
    return m().areaDrawable();
}

const utils::BoundingBox &LightNode::areaBoundingBox() const
{
    auto &mPrivate = m();

    if (mPrivate.boundingBoxPolicy() == BoundingBoxPolicy::Root)
        return boundingBox();

    if (mPrivate.isAreaBoundingBoxDirty())
    {
        mPrivate.areaBoundingBox() = doAreaBoundingBox();
        mPrivate.isAreaBoundingBoxDirty() = false;
    }
    return mPrivate.areaBoundingBox();
}

void LightNode::recalculateAreaBoundingBox()
{
    m().isAreaBoundingBoxDirty() = true;
    dirtyBoundingBox();
}

utils::Transform LightNode::calculateShadowViewTransform(const utils::FrustumCornersInfo &cameraFrustumCornersInfo) const
{
    return doShadowViewTransform(cameraFrustumCornersInfo);
}

glm::mat4x4 LightNode::calculateShadowProjectionMatrix(const utils::Transform &shadowViewTransform,
                                                       const utils::FrustumCornersInfo &cameraFrustumCornersInfo,
                                                       const utils::Range &zRange) const
{
    return doShadowProjectionMatrix(shadowViewTransform, cameraFrustumCornersInfo, zRange);
}

LightNode::LightNode(std::unique_ptr<LightNodePrivate> lightNodePrivate)
    : Node(std::move(lightNodePrivate))
{
    setLightingEnabled(true);
    setShadingEnabled(true);
}

bool LightNode::canAttach(std::shared_ptr<Node>)
{
    LOG_ERROR << "It's forbidden to attach to light node \"" << name() << "\"";
    return false;
}

bool LightNode::canDetach(std::shared_ptr<Node>)
{
    LOG_ERROR << "It's forbidden to detach from light node \"" << name() << "\"";
    return false;
}

utils::Transform LightNode::doShadowViewTransform(const utils::FrustumCornersInfo &) const
{ return utils::Transform(); }

glm::mat4x4 LightNode::doShadowProjectionMatrix(const utils::Transform &, const utils::FrustumCornersInfo &, const utils::Range &) const
{ return glm::mat4x4(1.f); }

}
}

#include <utils/logger.h>

#include <core/lightnode.h>
#include <core/lightdrawable.h>
#include <core/scene.h>
#include <core/graphicsengine.h>

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

LightShadingMode LightNode::shadingMode() const
{
    return m().shadingMode();
}

void LightNode::setShadingMode(LightShadingMode value)
{
    m().shadingMode() = value;
    m().isShadowFrameBufferDirty() = true;
}

const glm::uvec2 &LightNode::shadowMapSize() const
{
    return m().shadowMapSize();
}

void LightNode::setShadowMapSize(const glm::uvec2 &value)
{
    m().shadowMapSize() = value;
    m().isShadowFrameBufferDirty() = true;
}

const utils::Range &LightNode::shadowCullPlanesLimits() const
{
    return m().shadowCullPlanesLimits();
}

void LightNode::setShadowCullPlanesLimits(const utils::Range &value)
{
    if (value.nearValue() <= 0.f)
        LOG_CRITICAL << "Znear must be greater than 0.0";

    if (value.farValue() <= value.nearValue())
        LOG_CRITICAL << "Zfar must be greater than Znear";

    m().shadowCullPlanesLimits() = value;
}

void LightNode::updateShadowFrameBuffer()
{
    auto parentScene = scene();
    if (!parentScene)
        return;

    auto graphicsEngine = parentScene->graphicsEngine();
    if (graphicsEngine.expired())
        return;

    auto graphicsRenderer = graphicsEngine.lock()->graphicsRenderer();
    if (!graphicsRenderer)
        return;

    auto &mPrivate = m();

    auto &shadowFrameBuffer = mPrivate.shadowFrameBuffer();
    if (!shadowFrameBuffer)
        shadowFrameBuffer = mPrivate.createShadowFrameBuffer(graphicsRenderer);

    if (mPrivate.isShadowFrameBufferDirty())
    {
        shadowFrameBuffer->resize(graphicsRenderer, mPrivate.shadowMapSize(), shadingMode() == LightShadingMode::Color);

        auto &areaDrawable = mPrivate.areaDrawable();
        areaDrawable->setShadowDepthMap(shadowFrameBuffer->depthTexture());
        areaDrawable->setShadowColorMap(shadowFrameBuffer->colorTexture());

        mPrivate.isShadowFrameBufferDirty() = false;
    }
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

std::vector<glm::mat4x4> LightNode::updateLayeredShadowMatrices(const utils::FrustumCorners &cameraFrustumCorners,
                                                                const utils::Range &zRange)
{
    std::vector<glm::mat4x4> layeredShadowMatrices;
    auto result = doUpdateLayeredShadowMatrices(cameraFrustumCorners, zRange, layeredShadowMatrices);

    auto &mPrivate = m();

    auto &layeredShadowMatricesBuffer = mPrivate.layeredShadowMatricesBuffer();
    if (!layeredShadowMatricesBuffer)
    {
        auto parentScene = scene();
        if (!parentScene)
            return layeredShadowMatrices;

        auto graphicsEngine = parentScene->graphicsEngine();
        if (graphicsEngine.expired())
            return layeredShadowMatrices;

        auto graphicsRenderer = graphicsEngine.lock()->graphicsRenderer();
        if (!graphicsRenderer)
            return layeredShadowMatrices;

        auto buffer = graphicsRenderer->createBuffer(0u);
        layeredShadowMatricesBuffer = graphicsRenderer->createBufferRange(buffer, 0u);
    }

    size_t bufferSize = sizeof(glm::mat4x4) * layeredShadowMatrices.size() + 4u * sizeof(uint32_t);
    auto bufferData = new uint8_t[bufferSize];
    auto p = bufferData;
    *reinterpret_cast<uint32_t*>(p) = static_cast<uint32_t>(layeredShadowMatrices.size());
    p += 4u * sizeof(uint32_t);
    for (size_t i = 0u; i < layeredShadowMatrices.size(); ++i)
    {
        *reinterpret_cast<glm::mat4x4*>(p) = layeredShadowMatrices[i];
        p += sizeof(glm::mat4x4);
    }
    layeredShadowMatricesBuffer->buffer()->resize(bufferSize, bufferData);
    delete [] bufferData;

    auto &drawable = mPrivate.areaDrawable();
    drawable->setShadowMatrix(mPrivate.shadowBiasMatrix() * result);

    return layeredShadowMatrices;
}

LightNode::LightNode(std::unique_ptr<LightNodePrivate> lightNodePrivate)
    : Node(std::move(lightNodePrivate))
{
    setLightingEnabled(true);
    setShadingMode(LightShadingMode::Color);
    setShadowMapSize(glm::uvec2(512u, 512u));
    setShadowCullPlanesLimits({.5f, std::numeric_limits<float>::max()});
}

bool LightNode::canAttach(const std::shared_ptr<Node>&)
{
    LOG_ERROR << "It's forbidden to attach to light node \"" << name() << "\"";
    return false;
}

bool LightNode::canDetach(const std::shared_ptr<Node>&)
{
    LOG_ERROR << "It's forbidden to detach from light node \"" << name() << "\"";
    return false;
}

}
}

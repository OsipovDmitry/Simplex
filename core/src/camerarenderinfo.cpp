#include <core/uniform.h>

#include "camerarenderinfo.h"

namespace simplex
{
namespace core
{

CameraRenderInfo::CameraRenderInfo()
    : StateSet()
{
    m_cameraBuffer = PCameraBuffer::element_type::create();
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CameraBuffer) = graphics::BufferRange::create(m_cameraBuffer->buffer());

    m_clusterNodesBuffer = PClusterNodesBuffer::element_type::create();
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::ClusterNodesBuffer) = graphics::BufferRange::create(m_clusterNodesBuffer->buffer());

    m_lightNodesBuffer = PLightNodesBuffer::element_type::create();
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::LightNodesBuffer) = graphics::BufferRange::create(m_lightNodesBuffer->buffer());

    m_sceneInfoBuffer = PSceneInfoBuffer::element_type::create();
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::SceneInfoBuffer) = graphics::BufferRange::create(m_sceneInfoBuffer->buffer());
}

CameraRenderInfo::~CameraRenderInfo() = default;

void CameraRenderInfo::setFrustum(const utils::Frustum& value)
{
    m_cameraBuffer->set(CameraDescription::make(value));
}

void CameraRenderInfo::resetSceneInfo(
    const glm::uvec3& clusterSize,
    uint32_t drawDataCount,
    uint32_t lightsCount,
    uint32_t OITNodesMaxCount)
{
    m_sceneInfoBuffer->set(SceneInfoDescription::make(
        clusterSize,
        drawDataCount,
        lightsCount,
        OITNodesMaxCount,
        glm::compMul(clusterSize) * lightsCount));
}

PSceneInfoConstBuffer CameraRenderInfo::sceneInfoBuffer() const
{
    return m_sceneInfoBuffer;
}

utils::Range CameraRenderInfo::sceneInfoZRange() const
{
    auto ZRange = m_sceneInfoBuffer->getField<decltype(SceneInfoDescription::ZRange)>(offsetof(SceneInfoDescription, ZRange));
    return utils::Range(glm::uintBitsToFloat(ZRange[0u]), glm::uintBitsToFloat(ZRange[1u]));
}

}
}

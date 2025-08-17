#include <utils/glm/gtc/matrix_inverse.hpp>

#include <core/uniform.h>

#include "geometryrenderinfo.h"
#include "geometrybuffer.h"

namespace simplex
{
namespace core
{

GeometryRenderInfo::GeometryRenderInfo()
    : StateSet()
    , m_viewMatrix(1.f)
    , m_projectionMatrix(1.f)
{
}

GeometryRenderInfo::~GeometryRenderInfo() = default;

const glm::mat4x4& GeometryRenderInfo::viewMatrix() const
{
    return m_viewMatrix;
}

void GeometryRenderInfo::setViewMatrix(const glm::mat4& vm)
{
    m_viewMatrix = vm;
    const auto vmInv = glm::inverse(vm);

    getOrCreateUniform(core::UniformId::ViewMatrix) = makeUniform(vm);
    getOrCreateUniform(core::UniformId::ViewMatrixInverse) = makeUniform(vmInv);
    getOrCreateUniform(core::UniformId::ViewPosition) = makeUniform(glm::vec3(vmInv * glm::vec4(0.f, 0.f, 0.f, 1.f)));
    getOrCreateUniform(core::UniformId::ViewXDirection) = makeUniform(glm::vec3(vmInv * glm::vec4(1.f, 0.f, 0.f, 0.f)));
    getOrCreateUniform(core::UniformId::ViewYDirection) = makeUniform(glm::vec3(vmInv * glm::vec4(0.f, 1.f, 0.f, 0.f)));
    getOrCreateUniform(core::UniformId::ViewZDirection) = makeUniform(glm::vec3(vmInv * glm::vec4(0.f, 0.f, 1.f, 0.f)));

    setViewProjectionMatrix(vm, m_projectionMatrix);
}

const glm::mat4x4& GeometryRenderInfo::projectionMatrix() const
{
    return m_projectionMatrix;
}

void GeometryRenderInfo::setProjectionMatrix(const glm::mat4& pm)
{
    m_projectionMatrix = pm;

    getOrCreateUniform(core::UniformId::ProjectionMatrix) = makeUniform(pm);
    getOrCreateUniform(core::UniformId::ProjectionMatrixInverse) = makeUniform(glm::inverse(pm));

    setViewProjectionMatrix(m_viewMatrix, pm);
}

std::shared_ptr<const GeometryBuffer> GeometryRenderInfo::geometryBuffer() const
{
    return m_geometryBuffer;
}

void GeometryRenderInfo::setGeometryBuffer(const std::shared_ptr<const GeometryBuffer>& geometryBuffer)
{
    m_geometryBuffer = geometryBuffer;

    getOrCreateUniform(core::UniformId::ViewportSize) = makeUniform(geometryBuffer->size());
    getOrCreateUniform(UniformId::GBufferColorMap0) = makeUniform(geometryBuffer->colorTexture0());
    getOrCreateUniform(UniformId::GBufferColorMap1) = makeUniform(geometryBuffer->colorTexture1());
    getOrCreateUniform(UniformId::GBufferColorMap2) = makeUniform(geometryBuffer->colorTexture2());
    getOrCreateUniform(UniformId::GBufferDepthMap) = makeUniform(geometryBuffer->depthTexture());
    getOrCreateUniform(UniformId::OITDepthImage) = makeUniform(geometryBuffer->OITDepthTexture());
    getOrCreateUniform(UniformId::OITIndicesImage) = makeUniform(geometryBuffer->OITIndicesTexture());
    getOrCreateUniform(UniformId::GBufferFinalMap) = makeUniform(geometryBuffer->finalTexture());

    getOrCreateSSBO(SSBOId::OITNodesBuffer) = graphics::BufferRange::create(geometryBuffer->OITNodesBuffer()->buffer());
}

void GeometryRenderInfo::setSSAOMap(const graphics::PConstTexture &value)
{
    getOrCreateUniform(UniformId::SSAOMap) = makeUniform(value);
}

void GeometryRenderInfo::setViewProjectionMatrix(const glm::mat4x4& vm, const glm::mat4x4& pm)
{
    const auto vp = pm * vm;
    getOrCreateUniform(core::UniformId::ViewProjectionMatrix) = makeUniform(vp);
    getOrCreateUniform(core::UniformId::ViewProjectionMatrixInverse) = makeUniform(glm::inverse(vp));
}

}
}

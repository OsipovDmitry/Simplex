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

    getOrCreateUniform(UniformID::ViewMatrix) = makeUniform(vm);
    getOrCreateUniform(UniformID::ViewMatrixInverse) = makeUniform(vmInv);
    getOrCreateUniform(UniformID::ViewPosition) = makeUniform(glm::vec3(vmInv * glm::vec4(0.f, 0.f, 0.f, 1.f)));
    getOrCreateUniform(UniformID::ViewXDirection) = makeUniform(glm::vec3(vmInv * glm::vec4(1.f, 0.f, 0.f, 0.f)));
    getOrCreateUniform(UniformID::ViewYDirection) = makeUniform(glm::vec3(vmInv * glm::vec4(0.f, 1.f, 0.f, 0.f)));
    getOrCreateUniform(UniformID::ViewZDirection) = makeUniform(glm::vec3(vmInv * glm::vec4(0.f, 0.f, 1.f, 0.f)));

    setViewProjectionMatrix(vm, m_projectionMatrix);
}

const glm::mat4x4& GeometryRenderInfo::projectionMatrix() const
{
    return m_projectionMatrix;
}

void GeometryRenderInfo::setProjectionMatrix(const glm::mat4& pm)
{
    m_projectionMatrix = pm;

    getOrCreateUniform(UniformID::ProjectionMatrix) = makeUniform(pm);
    getOrCreateUniform(UniformID::ProjectionMatrixInverse) = makeUniform(glm::inverse(pm));

    setViewProjectionMatrix(m_viewMatrix, pm);
}

std::shared_ptr<GeometryBuffer> GeometryRenderInfo::geometryBuffer()
{
    return m_geometryBuffer;
}

std::shared_ptr<const GeometryBuffer> GeometryRenderInfo::geometryBuffer() const
{
    return const_cast<GeometryRenderInfo*>(this)->geometryBuffer();
}

void GeometryRenderInfo::setGeometryBuffer(const std::shared_ptr<GeometryBuffer>& geometryBuffer)
{
    m_geometryBuffer = geometryBuffer;

    getOrCreateUniform(UniformID::ViewportSize) = makeUniform(geometryBuffer->size());
    getOrCreateUniform(UniformID::GBufferColorMap0) = makeUniform(geometryBuffer->colorTexture0());
    getOrCreateUniform(UniformID::GBufferDepthMap) = makeUniform(geometryBuffer->depthTexture());
    getOrCreateUniform(UniformID::OITDepthMap) = makeUniform(geometryBuffer->OITDepthTexture());
    getOrCreateUniform(UniformID::OITIndicesMap) = makeUniform(geometryBuffer->OITIndicesTexture());
    getOrCreateUniform(UniformID::GBufferFinalMap) = makeUniform(geometryBuffer->finalTexture());
    
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::OITBuffer) = graphics::BufferRange::create(geometryBuffer->OITBuffer()->buffer());
}

void GeometryRenderInfo::setSSAOMap(const graphics::PConstTexture &value)
{
    getOrCreateUniform(UniformID::SSAOMap) = makeUniform(value);
}

void GeometryRenderInfo::setViewProjectionMatrix(const glm::mat4x4& vm, const glm::mat4x4& pm)
{
    const auto vp = pm * vm;
    getOrCreateUniform(UniformID::ViewProjectionMatrix) = makeUniform(vp);
    getOrCreateUniform(UniformID::ViewProjectionMatrixInverse) = makeUniform(glm::inverse(vp));
}

}
}

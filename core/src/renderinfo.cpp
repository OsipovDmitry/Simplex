#include <utils/logger.h>
#include <utils/glm/gtc/matrix_inverse.hpp>

#include <core/renderinfo.h>
#include <core/uniform.h>

namespace simplex
{
namespace core
{

RenderInfo::RenderInfo()
    : StateSet()
{
}

RenderInfo::~RenderInfo() = default;

void RenderInfo::setGBuffer(
    const graphics::PConstTexture& colorTexture0,
    const graphics::PConstTexture& colorTexture1,
    const graphics::PConstTexture& colorTexture2,
    const graphics::PConstTexture& depthTexture,
    const graphics::PConstTexture& OITDepthTexture,
    const graphics::PConstTexture& OITIndicesTexture,
    const graphics::PConstBuffer& OITNodesBuffer)
{
    getOrCreateUniform(UniformId::GBufferColorMap0) = makeUniform(colorTexture0);
    getOrCreateUniform(UniformId::GBufferColorMap1) = makeUniform(colorTexture1);
    getOrCreateUniform(UniformId::GBufferColorMap2) = makeUniform(colorTexture2);
    getOrCreateUniform(UniformId::GBufferDepthMap) = makeUniform(depthTexture);
    getOrCreateUniform(UniformId::OITDepthImage) = makeUniform(OITDepthTexture);
    getOrCreateUniform(UniformId::OITIndicesImage) = makeUniform(OITIndicesTexture);
    getOrCreateSSBO(SSBOId::OITNodesBuffer) = OITNodesBuffer;
}

void RenderInfo::setGBufferTmp(const graphics::PConstTexture &colorMap,
                            const graphics::PConstTexture &depthMap,
                            const graphics::PConstImage &depthImage,
                            const graphics::PConstImage &indicesImage,
                            const graphics::PConstBuffer &nodesBuffer)
{
    getOrCreateUniform(UniformId::GBufferColorMap0) = makeUniform(colorMap);
    getOrCreateUniform(UniformId::GBufferDepthMap) = makeUniform(depthMap);
    getOrCreateUniform(UniformId::OITDepthImage) = makeUniform(depthImage);
    getOrCreateUniform(UniformId::OITIndicesImage) = makeUniform(indicesImage);
    getOrCreateSSBO(SSBOId::OITNodesBuffer) = nodesBuffer;
}

void RenderInfo::setSSAOMap(const graphics::PConstTexture &value)
{
    getOrCreateUniform(UniformId::SSAOMap) = makeUniform(value);
}

void RenderInfo::setLayeredShadowMatricesBuffer(const graphics::PConstBuffer &value)
{
    getOrCreateSSBO(SSBOId::LayeredShadowMatrices) = value;
}

}
}

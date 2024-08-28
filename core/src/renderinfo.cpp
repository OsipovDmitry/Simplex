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

void RenderInfo::setGBuffer(const graphics::PConstTexture &colorMap,
                            const graphics::PConstTexture &depthMap,
                            const graphics::PConstImage &depthImage,
                            const graphics::PConstImage &indicesImage,
                            const graphics::PConstBufferRange &nodesBuffer,
                            const graphics::PConstBufferRange &nodesCounter)
{
    getOrCreateUniform(UniformId::GBufferColorMap) = makeUniform(colorMap);
    getOrCreateUniform(UniformId::GBufferDepthMap) = makeUniform(depthMap);
    getOrCreateUniform(UniformId::OITDepthImage) = makeUniform(depthImage);
    getOrCreateUniform(UniformId::OITIndicesImage) = makeUniform(indicesImage);
    getOrCreateSSBO(SSBOId::OITNodes) = nodesBuffer;
    getOrCreateUniform(UniformId::OITNodesCounter) = makeUniform(nodesCounter);
}

void RenderInfo::setSSAOMap(const graphics::PConstTexture &value)
{
    getOrCreateUniform(UniformId::SSAOMap) = makeUniform(value);
}

void RenderInfo::setLayeredShadowMatricesBuffer(const graphics::PConstBufferRange &value)
{
    getOrCreateSSBO(SSBOId::LayeredShadowMatrices) = value;
}

}
}

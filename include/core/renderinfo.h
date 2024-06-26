#ifndef CORE_RENDERINFO_H
#define CORE_RENDERINFO_H

#include <memory>

#include <utils/noncopyble.h>
#include <utils/glm/vec3.hpp>
#include <utils/glm/mat4x4.hpp>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class RenderInfoPrivate;
class CORE_SHARED_EXPORT RenderInfo
{
    NONCOPYBLE(RenderInfo)
public:
    RenderInfo();
    ~RenderInfo();

    const PUniform<glm::mat4x4> &viewMatrixUniform() const;
    const PUniform<glm::mat4x4> &viewMatrixInverseUniform() const;
    const PUniform<glm::vec3> &viewPositionUniform() const;
    const PUniform<glm::vec3> &viewXDirectionUniform() const;
    const PUniform<glm::vec3> &viewYDirectionUniform() const;
    const PUniform<glm::vec3> &viewZDirectionUniform() const;
    void setViewMatrix(const glm::mat4x4&);

    const PUniform<glm::mat4x4> &projectionMatrixUniform() const;
    const PUniform<glm::mat4x4> &projectionMatrixInverseUniform() const;
    void setProjectionMatrix(const glm::mat4x4&);

    const PUniform<glm::mat4x4> &viewProjectionMatrixUniform() const;
    const PUniform<glm::mat4x4> &viewProjectionMatrixInverseUniform() const;

    const PUniform<glm::vec2> &zRangeUniform() const;
    void setZRange(const glm::vec2&);

    const PUniform<graphics::PConstTexture> &GBufferColor0TextureUniform() const;
    const PUniform<graphics::PConstTexture> &GBufferColor1TextureUniform() const;
    const PUniform<graphics::PConstTexture> &GBufferColor2TextureUniform() const;
    const PUniform<graphics::PConstTexture> &GBufferDepthTextureUniform() const;
    void setGBuffer(const graphics::PConstTexture& color0Map,
                    const graphics::PConstTexture& color1Map,
                    const graphics::PConstTexture& color2Map,
                    const graphics::PConstTexture& depthMap);

    const PUniform<graphics::PConstImage> &OITDepthImageUniform() const;
    const PUniform<graphics::PConstImage> &OITIndicesImageUniform() const;
    const graphics::PConstBufferRange &OITNodesBuffer() const;
    const PUniform<graphics::PConstBufferRange> &OITNodesCounterUniform() const;
    void setOITBuffer(const graphics::PConstImage& depthImage,
                      const graphics::PConstImage& indicesImage,
                      const graphics::PConstBufferRange& nodesBuffer,
                      const graphics::PConstBufferRange& nodesCounter);

    const graphics::PConstBufferRange& layeredShadowMatricesBuffer() const;
    void setLayeredShadowMatricesBuffer(const graphics::PConstBufferRange&);

    const PUniform<graphics::PConstTexture> &lightBufferColorTextureUniform() const;
    void setLightBufferColorTexture(const graphics::PConstTexture&);

    const PUniform<graphics::PConstTexture> &finalBufferColorTextureUniform() const;
    void setFinalBufferColorTexture(const graphics::PConstTexture&);

private:
    std::unique_ptr<RenderInfoPrivate> m_;
};

}
}

#endif // CORE_RENDERINFO_H

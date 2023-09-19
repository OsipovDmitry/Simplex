#ifndef CORE_RENDERINFO_H
#define CORE_RENDERINFO_H

#include <memory>

#include <utils/noncopyble.h>
#include <utils/glm/vec3.hpp>
#include <utils/glm/mat4x4.hpp>

#include <core/coreglobal.h>
#include <core/igraphicsrenderer.h>

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

    const PUniform<graphics::PConstTexture> &gBufferColor0MapUniform() const;
    const PUniform<graphics::PConstTexture> &gBufferColor1MapUniform() const;
    const PUniform<graphics::PConstTexture> &gBufferDepthMapUniform() const;
    void setGBufferMaps(const graphics::PConstTexture& color0Map,
                        const graphics::PConstTexture& color1Map,
                        const graphics::PConstTexture& depthMap);

    const graphics::PConstBufferRange &OITNodesBuffer() const;
    void setOITNodesBuffer(const graphics::PConstBufferRange&);

    const PUniform<graphics::PConstBufferRange> &OITNodesCounterUniform() const;
    void setOITNodesCounter(const graphics::PConstBufferRange&);

    const PUniform<graphics::PConstImage> &OITIndicesImageUniform() const;
    void setOITIndicesImage(const graphics::PConstImage&);

    bool faceCulling() const;
    graphics::FaceType cullFaceType() const;
    void setFaceCulling(bool, graphics::FaceType = graphics::FaceType::Back);

    bool colorMask(uint16_t) const;
    void setColorMask(uint16_t, bool);
    void setColorMasks(bool);

    bool depthTest() const;
    graphics::DepthFunc depthFunc() const;
    void setDepthTest(bool, graphics::DepthFunc func = graphics::DepthFunc::Less);
    bool depthMask() const;
    void setDepthMask(bool);

    bool stencilTest() const;
    void setStencilTest(bool);

private:
    std::unique_ptr<RenderInfoPrivate> m_;
};

}
}

#endif // CORE_RENDERINFO_H

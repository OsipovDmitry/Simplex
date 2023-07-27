#ifndef CORE_RENDERINFO_H
#define CORE_RENDERINFO_H

#include <memory>

#include <utils/noncopyble.h>
#include <utils/glm/vec4.hpp>
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

    const std::shared_ptr<graphics::IFrameBuffer> frameBuffer() const;
    void setFrameBuffer(std::shared_ptr<graphics::IFrameBuffer>);

    const glm::uvec4 &viewport() const;
    void setViewport(const glm::uvec4&);

    const glm::mat4 &viewMatrix() const;
    const glm::mat4 &viewMatrixInverse() const;
    const glm::vec3 &viewPosition() const;
    const glm::vec3 &viewXDirection() const;
    const glm::vec3 &viewYDirection() const;
    const glm::vec3 &viewZDirection() const;
    void setViewMatrix(const glm::mat4&);

    const glm::mat4 &projectionMatrix() const;
    const glm::mat4 &projectionMatrixInverse() const;
    void setProjectionMatrix(const glm::mat4&);

    const glm::mat4 &viewProjectionMatrix() const;
    const glm::mat4 &viewProjectionMatrixInverse() const;

    const std::shared_ptr<const graphics::ITexture> &gBufferColor0Map() const;
    const std::shared_ptr<const graphics::ITexture> &gBufferColor1Map() const;
    const std::shared_ptr<const graphics::ITexture> &gBufferDepthMap() const;
    void setGBufferMaps(std::shared_ptr<const graphics::ITexture>,
                        std::shared_ptr<const graphics::ITexture>,
                        std::shared_ptr<const graphics::ITexture>);

    bool faceCulling() const;
    graphics::FaceType cullFaceType() const;
    void setFaceCulling(bool, graphics::FaceType = graphics::FaceType::Back);

    bool colorMask(uint16_t) const;
    void setColorMask(uint16_t, bool);

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

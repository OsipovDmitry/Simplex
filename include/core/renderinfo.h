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

    const std::shared_ptr<graphics::IRenderProgram> renderProgram() const;
    void setRenderProgram(std::shared_ptr<graphics::IRenderProgram>);

    const std::shared_ptr<graphics::IFrameBuffer> frameBuffer() const;
    void setFrameBuffer(std::shared_ptr<graphics::IFrameBuffer>);

    const glm::uvec4 &viewport() const;
    const PUniform<glm::uvec2> &viewportSizeUniform() const;
    void setViewport(const glm::uvec4&);

    const glm::vec4 &defaultVertexPosition() const;
    void setDefaultVertexPosition(const glm::vec4&);

    const glm::vec4 &defaultVertexNormal() const;
    void setDefaultVertexNormal(const glm::vec4&);

    const glm::vec4 &defaultVertexTexCoord() const;
    void setDefaultVertexTexCoord(const glm::vec4&);

    const glm::uvec4 &defaultVertexBonesIDs() const;
    void setDefaultVertexBonesIDs(const glm::uvec4&);

    const glm::vec4 &defaultVertexBonesWeights() const;
    void setDefaultVertexBonesWeights(const glm::vec4&);

    const glm::vec4 &defaultVertexTangent() const;
    void setDefaultVertexTangent(const glm::vec4&);

    const glm::vec4 &defaultVertexColor() const;
    void setDefaultVertexColor(const glm::vec4&);

    const PUniform<glm::vec4> &defaultBaseColorUniform() const;
    void setDefaultBaseColor(const glm::vec4&);

    const PUniform<float> &defaultMetalnessUniform() const;
    void setDefaultMetalness(float);

    const PUniform<float> &defaultRoughnessUniform() const;
    void setDefaultRoughness(float);

    const PUniform<graphics::PTexture> &defaultBaseColorMapUniform() const;
    void setDefaultBaseColorMap(const graphics::PTexture&);

    const PUniform<graphics::PTexture> &defaultMetalnessMapUniform() const;
    void setDefaultMetalnessMap(const graphics::PTexture&);

    const PUniform<graphics::PTexture> &defaultRoughnessMapUniform() const;
    void setDefaultRoughnessMap(const graphics::PTexture&);

    const PUniform<graphics::PTexture> &defaultNormalMapUniform() const;
    void setDefaultNormalMap(const graphics::PTexture&);

    const graphics::PBufferRange &defaultBonesBuffer() const;
    void setDefaultBonesBuffer(const graphics::PBufferRange&);

    const graphics::PBufferRange &OITNodesBuffer() const;
    void setOITNodesBuffer(const graphics::PBufferRange&);

    const PUniform<graphics::PBufferRange> &OITNodesCounterUniform() const;
    void setOITNodesCounter(const graphics::PBufferRange&);

    const PUniform<graphics::PImage> &OITIndicesImageUniform() const;
    void setOITIndicesImage(const graphics::PImage&);

    const PUniform<glm::mat4> &viewMatrixUniform() const;
    const PUniform<glm::mat4> &viewMatrixInverseUniform() const;
    const PUniform<glm::vec3> &viewPositionUniform() const;
    const PUniform<glm::vec3> &viewXDirectionUniform() const;
    const PUniform<glm::vec3> &viewYDirectionUniform() const;
    const PUniform<glm::vec3> &viewZDirectionUniform() const;
    void setViewMatrix(const glm::mat4&);

    const PUniform<glm::mat4> &projectionMatrixUniform() const;
    const PUniform<glm::mat4> &projectionMatrixInverseUniform() const;
    void setProjectionMatrix(const glm::mat4&);

    const PUniform<glm::mat4> &viewProjectionMatrixUniform() const;
    const PUniform<glm::mat4> &viewProjectionMatrixInverseUniform() const;

    const PUniform<graphics::PTexture> &gBufferColor0MapUniform() const;
    const PUniform<graphics::PTexture> &gBufferColor1MapUniform() const;
    const PUniform<graphics::PTexture> &gBufferDepthMapUniform() const;
    void setGBufferMaps(const graphics::PTexture&, const graphics::PTexture&, const graphics::PTexture&);

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

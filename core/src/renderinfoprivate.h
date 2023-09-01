#ifndef RENDERINFOPRIVATE_H
#define RENDERINFOPRIVATE_H

#include <memory>

#include <utils/glm/vec3.hpp>
#include <utils/glm/vec4.hpp>
#include <utils/glm/mat4x4.hpp>

#include <core/uniform.h>

namespace simplex
{
namespace core
{

class RenderInfoPrivate
{
public:
    RenderInfoPrivate() {}

    std::shared_ptr<graphics::IRenderProgram> &renderProgram() { return m_renderProgram; }
    std::shared_ptr<graphics::IFrameBuffer> &frameBuffer() { return m_frameBuffer; }
    glm::uvec4 &viewport() { return m_viewport; }
    PUniform<glm::uvec2> &viewportSizeUniform() { return m_viewportSizeUniform; }

    glm::vec4 &defaultVertexPosition() { return m_defaultVertexPosition; }
    glm::vec4 &defaultVertexNormal() { return m_defaultVertexNormal; }
    glm::vec4 &defaultVertexTexCoord() { return m_defaultVertexTexCoord; }
    glm::uvec4 &defaultVertexBonesIDs() { return m_defaultVertexBonesIDs; }
    glm::vec4 &defaultVertexBonesWeights() { return m_defaultVertexBonesWeights; }
    glm::vec4 &defaultVertexTangent() { return m_defaultVertexTangent; }
    glm::vec4 &defaultVertexColor() { return m_defaultVertexColor; }

    PUniform<glm::vec4> &defaultBaseColorUniform() { return m_defaultBaseColorUniform; }
    PUniform<float> &defaultMetallnessUniform() { return m_defaultMetallnessUniform; }
    PUniform<float> &defaultRoughnessUniform() { return m_defaultRoughnessUniform; }
    PUniform<graphics::PTexture> &defaultBaseColorMapUniform() { return m_defaultBaseColorMapUniform; }
    PUniform<graphics::PTexture> &defaultMetalnessMapUniform() { return m_defaultMetalnessMapUniform; }
    PUniform<graphics::PTexture> &defaultRoughnessMapUniform() { return m_defaultRoughnessMapUniform; }
    PUniform<graphics::PTexture> &defaultNormalMapUniform() { return m_defaultNormalMapUniform; }
    graphics::PBufferRange &defaultBonesBuffer() { return m_defaultBonesBuffer; }

    graphics::PBufferRange &OITDataBuffer() { return m_OITDataBuffer; }
    PUniform<graphics::PBufferRange> &OITNodesCounterUniform() { return m_OITNodesCounterUniform; }
    PUniform<graphics::PImage> &OITIndicesImageUniform() { return m_OITIndicesImageUniform; }

    PUniform<glm::mat4> &viewMatrixUniform() { return m_viewMatrixUniform; }
    PUniform<glm::mat4> &viewMatrixInverseUniform() { return m_viewMatrixInverseUniform; }
    PUniform<glm::mat4> &projectionMatrixUniform() { return m_projectionMatrixUniform; }
    PUniform<glm::mat4> &projectionMatrixInverseUniform() { return m_projectionMatrixInverseUniform; }
    PUniform<glm::mat4> &viewProjectionMatrixUniform() { return m_viewProjectionMatrixUniform; }
    PUniform<glm::mat4> &viewProjectionMatrixInverseUniform() { return m_viewProjectionMatrixInverseUniform; }
    PUniform<glm::vec3> &viewPositionUniform() { return m_viewPositionUniform; }
    PUniform<glm::vec3> &viewXDirectionUniform() { return m_viewXDirectionUniform; }
    PUniform<glm::vec3> &viewYDirectionUniform() { return m_viewYDirectionUniform; }
    PUniform<glm::vec3> &viewZDirectionUniform() { return m_viewZDirectionUniform; }

    PUniform<graphics::PTexture> &gBufferColor0MapUniform() { return m_gBufferColor0MapUniform; }
    PUniform<graphics::PTexture> &gBufferColor1MapUniform() { return m_gBufferColor1MapUniform; }
    PUniform<graphics::PTexture> &gBufferDepthMapUniform() { return m_gBufferDepthMapUniform; }

    bool &faceCulling() { return m_faceCulling; }
    graphics::FaceType &cullFaceType() { return m_cullFaceType; }

    std::array<bool, graphics::FrameBufferColorAttachmentsCount()> & colorMasks() { return m_colorMasks; }

    bool &depthTest() { return m_depthTest; }
    graphics::DepthFunc &depthFunc() { return m_depthFunc; }
    bool &depthMask() { return m_depthMask; }

    bool &stencilTest() { return m_stencilTest; }

private:
    std::shared_ptr<graphics::IRenderProgram> m_renderProgram;
    std::shared_ptr<graphics::IFrameBuffer> m_frameBuffer;
    glm::uvec4 m_viewport;
    PUniform<glm::uvec2> m_viewportSizeUniform;

    glm::vec4 m_defaultVertexPosition;
    glm::vec4 m_defaultVertexNormal;
    glm::vec4 m_defaultVertexTexCoord;
    glm::uvec4 m_defaultVertexBonesIDs;
    glm::vec4 m_defaultVertexBonesWeights;
    glm::vec4 m_defaultVertexTangent;
    glm::vec4 m_defaultVertexColor;

    PUniform<glm::vec4> m_defaultBaseColorUniform;
    PUniform<float> m_defaultMetallnessUniform;
    PUniform<float> m_defaultRoughnessUniform;
    PUniform<graphics::PTexture> m_defaultBaseColorMapUniform;
    PUniform<graphics::PTexture> m_defaultMetalnessMapUniform;
    PUniform<graphics::PTexture> m_defaultRoughnessMapUniform;
    PUniform<graphics::PTexture> m_defaultNormalMapUniform;
    graphics::PBufferRange m_defaultBonesBuffer;

    graphics::PBufferRange m_OITDataBuffer;
    PUniform<graphics::PBufferRange> m_OITNodesCounterUniform;
    PUniform<graphics::PImage> m_OITIndicesImageUniform;

    PUniform<glm::mat4> m_viewMatrixUniform;
    PUniform<glm::mat4> m_viewMatrixInverseUniform;
    PUniform<glm::mat4> m_projectionMatrixUniform;
    PUniform<glm::mat4> m_projectionMatrixInverseUniform;
    PUniform<glm::mat4> m_viewProjectionMatrixUniform;
    PUniform<glm::mat4> m_viewProjectionMatrixInverseUniform;
    PUniform<glm::vec3> m_viewPositionUniform;
    PUniform<glm::vec3> m_viewXDirectionUniform;
    PUniform<glm::vec3> m_viewYDirectionUniform;
    PUniform<glm::vec3> m_viewZDirectionUniform;

    PUniform<graphics::PTexture> m_gBufferColor0MapUniform;
    PUniform<graphics::PTexture> m_gBufferColor1MapUniform;
    PUniform<graphics::PTexture> m_gBufferDepthMapUniform;

    bool m_faceCulling;
    graphics::FaceType m_cullFaceType;

    std::array<bool, graphics::FrameBufferColorAttachmentsCount()> m_colorMasks;
    bool m_depthTest;
    graphics::DepthFunc m_depthFunc;
    bool m_depthMask;
    bool m_stencilTest;

};

}
}

#endif // RENDERINFOPRIVATE_H

#include <utils/mesh.h>

#include <core/stateset.h>
#include <core/graphicsrendererbase.h>

#include "graphicsengineprivate.h"

namespace simplex
{
namespace core
{

GraphicsEnginePrivate::GraphicsEnginePrivate(const std::string &name)
    : m_name(name)
{
}

std::string& GraphicsEnginePrivate::name()
{
    return m_name;
}

std::shared_ptr<graphics::RendererBase> &GraphicsEnginePrivate::renderer()
{
    return m_renderer;
}

std::shared_ptr<TexturesLoader> &GraphicsEnginePrivate::texturesLoader()
{
    return m_texturesLoader;
}

std::shared_ptr<ProgramsLoader> &GraphicsEnginePrivate::programsLoader()
{
    return m_programsLoader;
}

std::shared_ptr<ScenesLoader>& GraphicsEnginePrivate::scenesLoader()
{
    return m_scenesLoader;
}

std::shared_ptr<RenderPipeLine>& GraphicsEnginePrivate::renderPipeLine()
{
    return m_renderPipeLine;
}

std::shared_ptr<graphics::IFrameBuffer>& GraphicsEnginePrivate::frameBuffer()
{
    return m_frameBuffer;
}

std::shared_ptr<graphics::IVertexArray>& GraphicsEnginePrivate::vertexArray()
{
    return m_vertexArray;
}

std::shared_ptr<GeometryBuffer>& GraphicsEnginePrivate::geometryBuffer()
{
    return m_geometryBuffer;
}

const std::string &GraphicsEnginePrivate::attributeNameByID(utils::VertexAttribute ID)
{
    static const std::string s_undefinedId = "";
    static const std::unordered_map<utils::VertexAttribute, std::string> s_table {
        { utils::VertexAttribute::Position, "a_position" },
        { utils::VertexAttribute::Normal, "a_normal" },
        { utils::VertexAttribute::TexCoords, "a_texCoord" },
        { utils::VertexAttribute::Tangent, "a_tangent" },
        { utils::VertexAttribute::BonesIDs, "a_bonesIDs" },
        { utils::VertexAttribute::BonesWeights, "a_bonesWeights" },
        { utils::VertexAttribute::Color, "a_color" },
    };

    return s_table.count(ID) ? s_table.at(ID) : s_undefinedId;
}

const std::string& GraphicsEnginePrivate::outputNameByID(graphics::FrameBufferAttachment ID)
{
    static const std::string s_undefinedId = "";
    static const std::unordered_map<graphics::FrameBufferAttachment, std::string> s_table{
        { graphics::FrameBufferAttachment::Color0, "o_fragColor0" },
        { graphics::FrameBufferAttachment::Color1, "o_fragColor1" },
        { graphics::FrameBufferAttachment::Color2, "o_fragColor2" },
        { graphics::FrameBufferAttachment::Color3, "o_fragColor3" },
    };

    return s_table.count(ID) ? s_table.at(ID) : s_undefinedId;
}

const std::string &GraphicsEnginePrivate::uniformNameByID(UniformID ID)
{
    static const std::string s_undefinedId = "";
    static const std::unordered_map<UniformID, std::string> s_table {
        { UniformID::ViewportSize, "u_viewportSize" },
        { UniformID::ModelMatrix, "u_modelMatrix" },
        { UniformID::NormalMatrix, "u_normalMatrix" },
        { UniformID::NormalMatrixInverse, "u_normalMatrixInverse" },
        { UniformID::ViewMatrix, "u_viewMatrix" },
        { UniformID::ViewMatrixInverse, "u_viewMatrixInverse" },
        { UniformID::ProjectionMatrix, "u_projectionMatrix" },
        { UniformID::ProjectionMatrixInverse, "u_projectionMatrixInverse" },
        { UniformID::ViewProjectionMatrix, "u_viewProjectionMatrix" },
        { UniformID::ViewProjectionMatrixInverse, "u_viewProjectionMatrixInverse" },
        { UniformID::ModelPosition, "u_modelPosition" },
        { UniformID::ModelXDirection, "u_modelXDirection" },
        { UniformID::ModelYDirection, "u_modelYDirection" },
        { UniformID::ModelZDirection, "u_modelZDirection" },
        { UniformID::ViewPosition, "u_viewPosition" },
        { UniformID::ViewXDirection, "u_viewXDirection" },
        { UniformID::ViewYDirection, "u_viewYDirection" },
        { UniformID::ViewZDirection, "u_viewZDirection" },
        { UniformID::BackgroundColorMap, "u_backgroundColorMap" },
        { UniformID::BackgroundColor, "u_backgroundColor" },
        { UniformID::BackgroundRoughness, "u_backgroundRoughness" },
        { UniformID::GBufferColorMap0, "u_GBufferColorMap0" },
        { UniformID::GBufferColorMap1, "u_GBufferColorMap1" },
        { UniformID::GBufferColorMap2, "u_GBufferColorMap2" },
        { UniformID::GBufferDepthMap, "u_GBufferDepthMap" },
        { UniformID::OITIndicesMap, "u_OITIndicesMap" },
        { UniformID::GBufferFinalMap, "u_GBufferFinalMap" },
        { UniformID::SSAOMap, "u_SSAOMap" },
        { UniformID::SSAOContribution, "u_SSAOContribution" },
        { UniformID::SSAONoiseMap, "u_SSAONoiseMap" },
        { UniformID::SSAORadius, "u_SSAORadius" },
        { UniformID::AlphaCutoff, "u_alphaCutoff" },
        { UniformID::IsLightedFlag, "u_isLightedFlag" },
        { UniformID::IsShadowedFlag, "u_isShadowedFlag" },
        { UniformID::ORMSwizzleMask, "u_ORMSwizzleMask" },
        { UniformID::BaseColor, "u_baseColor" },
        { UniformID::BaseColorMap, "u_baseColorMap" },
        { UniformID::Metalness, "u_metalness" },
        { UniformID::MetalnessMap, "u_metalnessMap" },
        { UniformID::Roughness, "u_roughness" },
        { UniformID::RoughnessMap, "u_roughnessMap" },
        { UniformID::NormalMap, "u_normalMap" },
        { UniformID::NormalMapScale, "u_normalMapScale" },
        { UniformID::OcclusionMap, "u_occlusionMap" },
        { UniformID::OcclusionMapStrength, "u_occlusionMapStrength" },
        { UniformID::Emission, "u_emission" },
        { UniformID::EmissionMap, "u_emissionMap" },
        { UniformID::LightDielecticSpecular, "u_lightDielectricSpecular" },
        { UniformID::LightColor, "u_lightColor" },
        { UniformID::LightRadiuses, "u_lightRadiuses" },
        { UniformID::LightCosHalfAngles, "u_lightCosHalfAngles" },
        { UniformID::IBLBRDFLutMap, "u_IBLBRDFLutMap" },
        { UniformID::IBLDiffuseMap, "u_IBLDiffuseMap" },
        { UniformID::IBLSpecularMap, "u_IBLSpecularMap" },
        { UniformID::IBLContribution, "u_IBLContribution" },
        { UniformID::ShadowDepthMap, "u_shadowDepthMap" },
        { UniformID::ShadowColorMap, "u_shadowColorMap" },
        { UniformID::ShadowViewMatrix, "u_shadowViewMatrix" },
        { UniformID::ShadowProjectionMatrix, "u_shadowProjectionMatrix" },
        { UniformID::ShadowDepthBias, "u_shadowDepthBias" },
        { UniformID::SourceImage, "u_sourceImage" },
        { UniformID::DestinationImage, "u_destinationImage" },
        { UniformID::BlurPassIndex, "u_blurPassIndex" },
        { UniformID::BlurSourceDepthMap, "u_sourceDepthMap" },
        { UniformID::BlurMaxDepthDifference, "u_blurMaxDepthDifference" }
    };

    return s_table.count(ID) ? s_table.at(ID) : s_undefinedId;
}

const std::string& GraphicsEnginePrivate::uniformBlockNameByID(UniformBlockID ID)
{
    static const std::string s_undefinedId = "";
    static const std::unordered_map<UniformBlockID, std::string> s_table {
    };

    return s_table.count(ID) ? s_table.at(ID) : s_undefinedId;
}

const std::string &GraphicsEnginePrivate::shaderStorageBlockNameByID(ShaderStorageBlockID ID)
{
    static const std::string s_undefinedId = "";
    static const std::unordered_map<ShaderStorageBlockID, std::string> s_table {
        { ShaderStorageBlockID::VerticesDataBuffer, "ssbo_verticesDataBuffer" },
        { ShaderStorageBlockID::ElementsDataBuffer, "ssbo_elementsDataBuffer" },
        { ShaderStorageBlockID::SkeletonsDataBuffer, "ssbo_skeletonsDataBuffer" },
        { ShaderStorageBlockID::BonesTransformsDataBuffer, "ssbo_bonesTransformsDataBuffer" },

        { ShaderStorageBlockID::MeshesBuffer, "ssbo_meshesBuffer" },
        { ShaderStorageBlockID::MaterialMapsBuffer, "ssbo_materialMapsBuffer" },
        { ShaderStorageBlockID::MaterialsBuffer, "ssbo_materialsBuffer" },
        { ShaderStorageBlockID::DrawablesBuffer, "ssbo_drawablesBuffer" },
        { ShaderStorageBlockID::BackgroundBuffer, "ssbo_backgroundBuffer" },
        { ShaderStorageBlockID::LightsBuffer, "ssbo_lightsBuffer" },
        { ShaderStorageBlockID::SkeletonsBuffer, "ssbo_skeletonsBuffer" },

        { ShaderStorageBlockID::DrawDataBuffer, "ssbo_drawDataBuffer" },
        { ShaderStorageBlockID::SkeletalAnimatedDataBuffer, "ssbo_skeletalAnimatedDataBuffer" },

        { ShaderStorageBlockID::CommandsBuffer, "ssbo_commandsBuffer" },
        { ShaderStorageBlockID::OpaqueCommandsBuffer, "ssbo_opaqueCommandsBuffer" },
        { ShaderStorageBlockID::TransparentCommandsBuffer, "ssbo_transparentCommandsBuffer" },

        { ShaderStorageBlockID::CameraBuffer, "ssbo_cameraBuffer" },
        { ShaderStorageBlockID::ClusterNodesBuffer, "ssbo_clusterNodesBuffer" },
        { ShaderStorageBlockID::LightNodesBuffer, "ssbo_lightNodesBuffer" },
        { ShaderStorageBlockID::SceneInfoBuffer, "ssbo_sceneInfoBuffer" },
        { ShaderStorageBlockID::GBuffer, "ssbo_GBuffer" },
        { ShaderStorageBlockID::OITNodesBuffer, "ssbo_OITNodesBuffer" },

        { ShaderStorageBlockID::LayeredShadowMatrices, "ssbo_layeredShadowMatricesBuffer" },
        { ShaderStorageBlockID::SSAOKernel, "ssbo_SSAOKernelBuffer" },
        { ShaderStorageBlockID::BlurKernel, "ssbo_blurKernelBuffer" }
    };

    return s_table.count(ID) ? s_table.at(ID) : s_undefinedId;
}

}
}

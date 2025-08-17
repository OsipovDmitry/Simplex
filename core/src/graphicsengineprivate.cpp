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

std::shared_ptr<TexturesManager> &GraphicsEnginePrivate::texturesManager()
{
    return m_texturesManager;
}

std::shared_ptr<ProgramsManager> &GraphicsEnginePrivate::programsManager()
{
    return m_programsManager;
}

std::shared_ptr<Scene>& GraphicsEnginePrivate::scene()
{
    return m_scene;
}

std::vector<std::shared_ptr<Pass>>& GraphicsEnginePrivate::passes()
{
    return m_passes;
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

std::shared_ptr<graphics::VAOMesh> &GraphicsEnginePrivate::screenQuadVertexArray()
{
    return m_screenQuadVertexArray;
}

std::shared_ptr<graphics::VAOMesh> &GraphicsEnginePrivate::pointLightAreaVertexArray()
{
    return m_pointLightAreaVertexArray;
}

utils::BoundingBox &GraphicsEnginePrivate::pointLightAreaBoundingBox()
{
    return m_pointLightAreaBoundingBox;
}

std::shared_ptr<graphics::VAOMesh> &GraphicsEnginePrivate::spotLightAreaVertexArray()
{
    return m_spotLightAreaVertexArray;
}

utils::BoundingBox &GraphicsEnginePrivate::spotLightAreaBoundingBox()
{
    return m_spotLightAreaBoundingBox;
}

std::shared_ptr<graphics::VAOMesh> &GraphicsEnginePrivate::directionalLightAreaVertexArray()
{
    return m_directionalLightAreaVertexArray;
}

std::shared_ptr<Drawable> &GraphicsEnginePrivate::screenQuadDrawable()
{
    return m_screenQuadDrawable;
}

std::shared_ptr<FlatDrawable> &GraphicsEnginePrivate::nodeBoundingBoxDrawable()
{
    return m_nodeBoundingBoxDrawable;
}

std::shared_ptr<FlatDrawable> &GraphicsEnginePrivate::visualDrawableNodeLocalBoundingBoxDrawable()
{
    return m_visualDrawableNodeLocalBoundingBoxDrawable;
}

std::shared_ptr<FlatDrawable> &GraphicsEnginePrivate::visualDrawableBoundingBoxDrawable()
{
    return m_visualDrawableBoundingBoxDrawable;
}

std::shared_ptr<FlatDrawable> &GraphicsEnginePrivate::lightNodeAreaBoundingBoxDrawable()
{
    return m_lightNodeAreaBoundingBoxDrawable;
}

const std::string &GraphicsEnginePrivate::attributeNameById(utils::VertexAttribute ID)
{
    static const std::string s_undefinedId = "";
    static const std::unordered_map<utils::VertexAttribute, std::string> s_table {
        { utils::VertexAttribute::Position, "a_position" },
        { utils::VertexAttribute::Normal, "a_normal" },
        { utils::VertexAttribute::TexCoords, "a_texCoord" },
        { utils::VertexAttribute::BonesIDs, "a_bonesIDs" },
        { utils::VertexAttribute::BonesWeights, "a_bonesWeights" },
        { utils::VertexAttribute::Tangent, "a_tangent" },
        { utils::VertexAttribute::Color, "a_color" },
    };

    return s_table.count(ID) ? s_table.at(ID) : s_undefinedId;
}

const std::string& GraphicsEnginePrivate::outputNameById(graphics::FrameBufferAttachment ID)
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

const std::string &GraphicsEnginePrivate::uniformNameById(UniformId ID)
{
    static const std::string s_undefinedId = "";
    static const std::unordered_map<UniformId, std::string> s_table {
        { UniformId::ViewportSize, "u_viewportSize" },
        { UniformId::ModelMatrix, "u_modelMatrix" },
        { UniformId::NormalMatrix, "u_normalMatrix" },
        { UniformId::NormalMatrixInverse, "u_normalMatrixInverse" },
        { UniformId::ViewMatrix, "u_viewMatrix" },
        { UniformId::ViewMatrixInverse, "u_viewMatrixInverse" },
        { UniformId::ProjectionMatrix, "u_projectionMatrix" },
        { UniformId::ProjectionMatrixInverse, "u_projectionMatrixInverse" },
        { UniformId::ViewProjectionMatrix, "u_viewProjectionMatrix" },
        { UniformId::ViewProjectionMatrixInverse, "u_viewProjectionMatrixInverse" },
        { UniformId::ModelPosition, "u_modelPosition" },
        { UniformId::ModelXDirection, "u_modelXDirection" },
        { UniformId::ModelYDirection, "u_modelYDirection" },
        { UniformId::ModelZDirection, "u_modelZDirection" },
        { UniformId::ViewPosition, "u_viewPosition" },
        { UniformId::ViewXDirection, "u_viewXDirection" },
        { UniformId::ViewYDirection, "u_viewYDirection" },
        { UniformId::ViewZDirection, "u_viewZDirection" },
        { UniformId::BackgroundColorMap, "u_backgroundColorMap" },
        { UniformId::BackgroundColor, "u_backgroundColor" },
        { UniformId::BackgroundRoughness, "u_backgroundRoughness" },
        { UniformId::GBufferColorMap0, "u_GBufferColorMap0" },
        { UniformId::GBufferColorMap1, "u_GBufferColorMap1" },
        { UniformId::GBufferColorMap2, "u_GBufferColorMap2" },
        { UniformId::GBufferDepthMap, "u_GBufferDepthMap" },
        { UniformId::OITDepthImage, "u_OITDepthImage" },
        { UniformId::OITIndicesImage, "u_OITIndicesImage" },
        { UniformId::GBufferFinalMap, "u_GBufferFinalMap" },
        { UniformId::SSAOMap, "u_SSAOMap" },
        { UniformId::SSAOContribution, "u_SSAOContribution" },
        { UniformId::SSAONoiseMap, "u_SSAONoiseMap" },
        { UniformId::SSAORadius, "u_SSAORadius" },
        { UniformId::AlphaCutoff, "u_alphaCutoff" },
        { UniformId::IsLightedFlag, "u_isLightedFlag" },
        { UniformId::IsShadowedFlag, "u_isShadowedFlag" },
        { UniformId::ORMSwizzleMask, "u_ORMSwizzleMask" },
        { UniformId::BaseColor, "u_baseColor" },
        { UniformId::BaseColorMap, "u_baseColorMap" },
        { UniformId::Metalness, "u_metalness" },
        { UniformId::MetalnessMap, "u_metalnessMap" },
        { UniformId::Roughness, "u_roughness" },
        { UniformId::RoughnessMap, "u_roughnessMap" },
        { UniformId::NormalMap, "u_normalMap" },
        { UniformId::NormalMapScale, "u_normalMapScale" },
        { UniformId::OcclusionMap, "u_occlusionMap" },
        { UniformId::OcclusionMapStrength, "u_occlusionMapStrength" },
        { UniformId::Emission, "u_emission" },
        { UniformId::EmissionMap, "u_emissionMap" },
        { UniformId::LightDielecticSpecular, "u_lightDielectricSpecular" },
        { UniformId::LightColor, "u_lightColor" },
        { UniformId::LightRadiuses, "u_lightRadiuses" },
        { UniformId::LightCosHalfAngles, "u_lightCosHalfAngles" },
        { UniformId::IBLBRDFLutMap, "u_IBLBRDFLutMap" },
        { UniformId::IBLDiffuseMap, "u_IBLDiffuseMap" },
        { UniformId::IBLSpecularMap, "u_IBLSpecularMap" },
        { UniformId::IBLContribution, "u_IBLContribution" },
        { UniformId::ShadowDepthMap, "u_shadowDepthMap" },
        { UniformId::ShadowColorMap, "u_shadowColorMap" },
        { UniformId::ShadowViewMatrix, "u_shadowViewMatrix" },
        { UniformId::ShadowProjectionMatrix, "u_shadowProjectionMatrix" },
        { UniformId::ShadowDepthBias, "u_shadowDepthBias" },
        { UniformId::SourceImage, "u_sourceImage" },
        { UniformId::DestinationImage, "u_destinationImage" },
        { UniformId::BlurPassIndex, "u_blurPassIndex" },
        { UniformId::BlurSourceDepthMap, "u_sourceDepthMap" },
        { UniformId::BlurMaxDepthDifference, "u_blurMaxDepthDifference" }
    };

    return s_table.count(ID) ? s_table.at(ID) : s_undefinedId;
}

const std::string &GraphicsEnginePrivate::SSBONameById(SSBOId ID)
{
    static const std::string s_undefinedId = "";
    static const std::unordered_map<SSBOId, std::string> s_table {
        { SSBOId::PositionsBuffer, "ssbo_positionsBuffer" },
        { SSBOId::NormalsBuffer, "ssbo_normalsBuffer" },
        { SSBOId::TexCoordsBuffer, "ssbo_texCoordsBuffer" },
        { SSBOId::BonesBuffer, "ssbo_bonesBuffer" },
        { SSBOId::TangentsBuffer, "ssbo_tangentsBuffer" },
        { SSBOId::ColorsBuffer, "ssbo_colorsBuffer" },
        { SSBOId::IndicesBuffer, "ssbo_indicesBuffer" },
        { SSBOId::MeshesBuffer, "ssbo_meshesBuffer" },
        { SSBOId::MaterialMapsBuffer, "ssbo_materialMapsBuffer" },
        { SSBOId::MaterialsBuffer, "ssbo_materialsBuffer" },
        { SSBOId::DrawablesBuffer, "ssbo_drawablesBuffer" },
        { SSBOId::DrawDataBuffer, "ssbo_drawDataBuffer" },
        { SSBOId::BackgroundsBuffer, "ssbo_backgroundsBuffer" },
        { SSBOId::CommandsBuffer, "ssbo_commandsBuffer" },
        { SSBOId::OpaqueCommandsBuffer, "ssbo_opaqueCommandsBuffer" },
        { SSBOId::TransparentCommandsBuffer, "ssbo_transparentCommandsBuffer" },
        { SSBOId::OITNodesBuffer, "ssbo_OITNodesBuffer" },
        { SSBOId::LayeredShadowMatrices, "ssbo_layeredShadowMatricesBuffer" },
        { SSBOId::SSAOKernel, "ssbo_SSAOKernelBuffer" },
        { SSBOId::BlurKernel, "ssbo_blurKernelBuffer" }
    };

    return s_table.count(ID) ? s_table.at(ID) : s_undefinedId;
}

}
}

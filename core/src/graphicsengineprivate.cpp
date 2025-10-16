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
        { UniformID::OITDepthMap, "u_OITDepthMap" },
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
        { ShaderStorageBlockID::VertexDataBuffer, "ssbo_vertexDataBuffer" },
        { ShaderStorageBlockID::ElementsBuffer, "ssbo_elementsBuffer" },
        { ShaderStorageBlockID::BonesBuffer, "ssbo_bonesBuffer" },
        { ShaderStorageBlockID::MeshesBuffer, "ssbo_meshesBuffer" },
        { ShaderStorageBlockID::MaterialMapsBuffer, "ssbo_materialMapsBuffer" },
        { ShaderStorageBlockID::MaterialsBuffer, "ssbo_materialsBuffer" },
        { ShaderStorageBlockID::DrawablesBuffer, "ssbo_drawablesBuffer" },
        { ShaderStorageBlockID::DrawDataBuffer, "ssbo_drawDataBuffer" },
        { ShaderStorageBlockID::BackgroundBuffer, "ssbo_backgroundBuffer" },
        { ShaderStorageBlockID::LightsInfoBuffer, "ssbo_lightsInfoBuffer" },
        { ShaderStorageBlockID::PointLightsBuffer, "ssbo_pointLightsBuffer" },
        { ShaderStorageBlockID::SpotLightsBuffer, "ssbo_spotLightsBuffer" },
        { ShaderStorageBlockID::DirectionalLightsBuffer, "ssbo_directionalLightsBuffer" },
        { ShaderStorageBlockID::ImageBasedLightsBuffer, "ssbo_imageBasedLightsBuffer" },
        { ShaderStorageBlockID::CommandsBuffer, "ssbo_commandsBuffer" },
        { ShaderStorageBlockID::OpaqueCommandsBuffer, "ssbo_opaqueCommandsBuffer" },
        { ShaderStorageBlockID::TransparentCommandsBuffer, "ssbo_transparentCommandsBuffer" },
        { ShaderStorageBlockID::OITBuffer, "ssbo_OITBuffer" },
        { ShaderStorageBlockID::LayeredShadowMatrices, "ssbo_layeredShadowMatricesBuffer" },
        { ShaderStorageBlockID::SSAOKernel, "ssbo_SSAOKernelBuffer" },
        { ShaderStorageBlockID::BlurKernel, "ssbo_blurKernelBuffer" }
    };

    return s_table.count(ID) ? s_table.at(ID) : s_undefinedId;
}

}
}

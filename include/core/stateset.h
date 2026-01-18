#ifndef CORE_STATESET_H
#define CORE_STATESET_H

#include <utils/noncopyble.h>
#include <utils/enumclass.h>
#include <utils/pimpl.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

ENUMCLASS(UniformID, uint16_t,
          ViewportSize,
          ModelMatrix,
          NormalMatrix,
          NormalMatrixInverse,
          ViewMatrix,
          ViewMatrixInverse,
          ProjectionMatrix,
          ProjectionMatrixInverse,
          ViewProjectionMatrix,
          ViewProjectionMatrixInverse,
          ModelPosition,
          ModelXDirection,
          ModelYDirection,
          ModelZDirection,
          ViewPosition,
          ViewXDirection,
          ViewYDirection,
          ViewZDirection,
          BackgroundColorMap,
          BackgroundColor,
          BackgroundRoughness,
          GBufferColorMap0,
          GBufferColorMap1,
          GBufferColorMap2,
          GBufferDepthMap,
          OITIndicesMap,
          GBufferFinalMap,
          SSAOMap,
          SSAOContribution,
          SSAONoiseMap,
          SSAORadius,
          AlphaCutoff,
          IsLightedFlag,
          IsShadowedFlag,
          ORMSwizzleMask,
          BaseColor,
          BaseColorMap,
          Metalness,
          MetalnessMap,
          Roughness,
          RoughnessMap,
          NormalMap,
          NormalMapScale,
          OcclusionMap,
          OcclusionMapStrength,
          Emission,
          EmissionMap,
          LightDielecticSpecular,
          LightColor,
          LightRadiuses,
          LightCosHalfAngles,
          IBLBRDFLutMap,
          IBLDiffuseMap,
          IBLSpecularMap,
          IBLContribution,
          ShadowDepthMap,
          ShadowColorMap,
          ShadowViewMatrix,
          ShadowProjectionMatrix,
          ShadowDepthBias,
          SourceImage,
          DestinationImage,
          BlurPassIndex,
          BlurSourceDepthMap,
          BlurMaxDepthDifference)

ENUMCLASS(UniformBlockID, uint16_t,
    None)

ENUMCLASS(ShaderStorageBlockID, uint16_t,
    VertexDataBuffer,
    ElementsBuffer,
    BonesBuffer,

    MeshesBuffer,
    MaterialMapsBuffer,
    MaterialsBuffer,
    DrawablesBuffer,
    DrawDataBuffer,
    BackgroundBuffer,
    LightsBuffer,

    CommandsBuffer,
    OpaqueCommandsBuffer,
    TransparentCommandsBuffer,

    CameraBuffer,
    ClusterNodesBuffer,
    LightNodesBuffer,
    SceneInfoBuffer,
    GBuffer,
    OITNodesBuffer,

    LayeredShadowMatrices,
    SSAOKernel,
    BlurKernel)

class StateSetPrivate;
class CORE_SHARED_EXPORT StateSet
{
    NONCOPYBLE(StateSet)

public:
    StateSet();
    virtual ~StateSet();

    const UniformCollection &uniformCollection() const;
    PConstAbstractUniform uniform(UniformID) const;
    PAbstractUniform uniform(UniformID);
    PAbstractUniform &getOrCreateUniform(UniformID);
    void removeUniform(UniformID);

    const UserUniformCollection &userUniformCollection() const;
    PConstAbstractUniform userUniform(const std::string&) const;
    PAbstractUniform userUniform(const std::string&);
    PAbstractUniform &getOrCreateUserUniform(const std::string&);
    void removeUserUniform(const std::string&);

    const UniformBlockCollection& uniformBlockCollection() const;
    graphics::PConstBufferRange uniformBlock(UniformBlockID) const;
    graphics::PConstBufferRange& getOrCreateUniformBlock(UniformBlockID);
    void removeUniformBlock(UniformBlockID);

    const ShaderStorageBlockCollection &shaderStorageBlockCollection() const;
    graphics::PConstBufferRange shaderStorageBlock(ShaderStorageBlockID) const;
    graphics::PConstBufferRange &getOrCreateShaderStorageBlock(ShaderStorageBlockID);
    void removeShaderStorageBlock(ShaderStorageBlockID);

protected:
    StateSet(std::unique_ptr<StateSetPrivate>);

    std::unique_ptr<StateSetPrivate> m_;
};

}
}

#endif // CORE_STATESET_H

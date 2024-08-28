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

ENUMCLASS(UniformId, uint16_t,
          Undefined,
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
          ModelViewMatrix,
          NormalViewMatrix,
          ModelViewProjectionMatrix,
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
          GBufferColorMap,
          GBufferDepthMap,
          OITDepthImage,
          OITIndicesImage,
          OITNodesCounter,
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

ENUMCLASS(SSBOId, uint16_t,
          Undefined,
          BonesBuffer,
          LayeredShadowMatrices,
          OITNodes,
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
    PConstAbstractUniform uniform(UniformId) const;
    PAbstractUniform uniform(UniformId);
    PAbstractUniform &getOrCreateUniform(UniformId);
    void removeUniform(UniformId);

    const UserUniformCollection &userUniformCollection() const;
    PConstAbstractUniform userUniform(const std::string&) const;
    PAbstractUniform userUniform(const std::string&);
    PAbstractUniform &getOrCreateUserUniform(const std::string&);
    void removeUserUniform(const std::string&);

    const SSBOCollection &ssboCollection() const;
    graphics::PConstBufferRange SSBO(SSBOId) const;
    graphics::PConstBufferRange &getOrCreateSSBO(SSBOId);
    void removeSSBO(SSBOId);

protected:
    StateSet(std::unique_ptr<StateSetPrivate>);

    std::unique_ptr<StateSetPrivate> m_;
};

}
}

#endif // CORE_STATESET_H

#ifndef CORE_SSAO_H
#define CORE_SSAO_H

#include <memory>

#include <utils/enumclass.h>
#include <utils/pimpl.h>

#include <core/coreglobal.h>

#include <inttypes.h>

namespace simplex
{
namespace core
{

// ENUMCLASS(SSAOMode, uint16_t,
//           Disabled, // No SSAO
//           Opaque, // opaque geometries cast balck shadow
//           OpaqueAndTransparent, // all the geometries cast black shadow
//           Color) // opaque geometries cast black shadow, transparent geometies cast color shadow
//
// class SSAOPrivate;
// class CORE_SHARED_EXPORT SSAO
//{
//     PRIVATE_IMPL(SSAO)
// public:
//     SSAO();
//     ~SSAO();
//
//     SSAOMode mode() const;
//     void setMode(SSAOMode);
//
//     float viewportDenominator() const;
//     void setViewportDenominator(float);
//
//     float contribution() const;
//     void setContribution(float);
//
//     float radius() const;
//     void setRadius(float);
//
//     uint32_t kernelBufferSize() const;
//     void setKernelBufferSize(uint32_t);
//
//     uint32_t noiseTextureSize() const;
//     void setNoiseTextureSize(uint32_t);
//
//     float blurSigma() const;
//     void setBlurSigma(float);
//
// private:
//     std::unique_ptr<SSAOPrivate> m_;
//
// };

}
} // namespace simplex

#endif // CORE_SSAO_H

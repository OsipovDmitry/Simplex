#include <utils/logger.h>

#include <core/igraphicswidget.h>
#include <core/graphicsengine.h>
#include <core/graphicsrendererbase.h>
#include <core/settings.h>
#include <core/ssao.h>

#include "ssaodrawable.h"
#include "bilaterialblur.h"
#include "graphicsengineprivate.h"
#include "ssaoprivate.h"

namespace simplex
{
namespace core
{

//SSAO::SSAO()
//    : m_(std::make_unique<SSAOPrivate>())
//{
//    auto graphicsRenderer = graphics::RendererBase::current();
//    if (!graphicsRenderer)
//        LOG_CRITICAL << "Graphics renderer can't be nullptr";
//
//    auto graphicsWidget = graphicsRenderer->widget();
//    if (!graphicsWidget)
//        LOG_CRITICAL << "Graphics widget can't be nullptr";
//
//    auto graphicsEngine = graphicsWidget->graphicsEngine();
//    if (!graphicsEngine)
//        LOG_CRITICAL << "Graphics engine can't be nullptr";
//
//    m_->drawable();// = std::make_unique<SSAODrawable>(graphicsEngine->m().screenQuadVertexArray());
//    m_->blur() = std::make_shared<BilaterialBlur>();
//
//    const auto &SSAOSettings = settings::Settings::instance().graphics().ssao();
//    setMode(SSAOSettings.mode());
//    setViewportDenominator(SSAOSettings.viewportDenominator());
//}
//
//SSAO::~SSAO() = default;
//
//SSAOMode SSAO::mode() const
//{
//    return m().mode();
//}
//
//void SSAO::setMode(SSAOMode value)
//{
//    m().mode() = value;
//}
//
//float SSAO::viewportDenominator() const
//{
//    return m().viewportDenominator();
//}
//
//void SSAO::setViewportDenominator(float value)
//{
//    if (value < 1.f)
//        LOG_CRITICAL << "SSAO viewport denominator must be greater than 1.0";
//
//    m().viewportDenominator() = value;
//}
//
//float SSAO::contribution() const
//{
//    return m().drawable()->contribution();
//}
//
//void SSAO::setContribution(float value)
//{
//    m().drawable()->setContribution(value);
//}
//
//float SSAO::radius() const
//{
//    return m().drawable()->radius();
//}
//
//void SSAO::setRadius(float value)
//{
//    m().drawable()->setRadius(value);
//}
//
//uint32_t SSAO::kernelBufferSize() const
//{
//    return m().drawable()->kernelBufferSize();
//}
//
//void SSAO::setKernelBufferSize(uint32_t value)
//{
//    m().drawable()->setKernelBufferSize(value);
//}
//
//uint32_t SSAO::noiseTextureSize() const
//{
//    return m().drawable()->noiseTextureSize();
//}
//
//void SSAO::setNoiseTextureSize(uint32_t value)
//{
//    m().drawable()->setNoiseTextureSize(value);
//}
//
//float SSAO::blurSigma() const
//{
//    return m().blur()->sigma();
//}
//
//void SSAO::setBlurSigma(float value)
//{
//    m().blur()->setSigma(value);
//}

}
}

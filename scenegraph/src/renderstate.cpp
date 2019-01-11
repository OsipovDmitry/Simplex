#include <scenegraph/renderstate.h>

#include "renderstate_p.h"

namespace scenegraph
{

void RenderState::enableDepthTest(types::DepthTestFunc func)
{
    m_->depthTestState = true;
    m_->depthTestFunc = func;
}

void RenderState::disableDepthTest()
{
    m_->depthTestState = false;
}

bool RenderState::depthTestState() const
{
    return m_->depthTestState;
}

types::DepthTestFunc RenderState::depthTestFunc() const
{
    return m_->depthTestFunc;
}

void RenderState::setDepthWriteState(bool state)
{
    m_->depthWriteState = state;
}

bool RenderState::depthWriteState() const
{
    return m_->depthWriteState;
}

void RenderState::setColorWriteState(bool r, bool g, bool b, bool a)
{
    m_->colorWriteState[0] = r;
    m_->colorWriteState[1] = g;
    m_->colorWriteState[2] = b;
    m_->colorWriteState[3] = a;
}

void RenderState::colorWriteState(bool &r, bool &g, bool &b, bool &a) const
{
    r = m_->colorWriteState[0];
    g = m_->colorWriteState[1];
    b = m_->colorWriteState[2];
    a = m_->colorWriteState[3];
}

void RenderState::enableBlend(types::BlendFunc srcRGB, types::BlendFunc srcA, types::BlendFunc dstRGB, types::BlendFunc dstA, types::BlendEquation eqRGB, types::BlendEquation eqA)
{
    m_->blendState = true;
    m_->blendFuncSrcRGB = srcRGB;
    m_->blendFuncSrcAlpha = srcA;
    m_->blendFuncDstRGB = dstRGB;
    m_->blendFuncDstAlpha = dstA;
    m_->blendEqRGB = eqRGB;
    m_->blendEqAlpha = eqA;
}

void RenderState::disableBlend()
{
    m_->blendState = false;
}

void RenderState::setBlendConstColor(const glm::vec4 &value)
{
    m_->blendConstColor = value;
}

glm::vec4 RenderState::blendConstColor() const
{
    return m_->blendConstColor;
}

bool RenderState::blendState() const
{
    return m_->blendState;
}

void RenderState::blenFunc(types::BlendFunc &srcRGB, types::BlendFunc &srcA, types::BlendFunc &dstRGB, types::BlendFunc &dstA) const
{
    srcRGB = m_->blendFuncSrcRGB;
    srcA = m_->blendFuncSrcAlpha;
    dstRGB = m_->blendFuncDstRGB;
    dstA = m_->blendFuncDstAlpha;
}

void RenderState::blendEquation(types::BlendEquation &eqRGB, types::BlendEquation &eqA) const
{
    eqRGB = m_->blendEqRGB;
    eqA = m_->blendEqAlpha;
}

RenderStatePtr RenderState::create()
{
    auto renderState = RenderStatePtr(new RenderState(), RenderStateDeleter());
    return renderState;
}

RenderState::RenderState() :
    m_(new RenderStatePrivate())
{
}

RenderState::~RenderState()
{
    delete m_;
}


}

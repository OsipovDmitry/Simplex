#include <renderer/context.h>

#include "renderstate_p.h"

namespace scenegraph
{

RenderStatePrivate::RenderStatePrivate() :
    layer(0u),
    depthTestFunc(types::DepthTestFunc::Less),
    blendFuncSrcRGB(types::BlendFunc::SourceAlpha),
    blendFuncSrcAlpha(types::BlendFunc::SourceAlpha),
    blendFuncDstRGB(types::BlendFunc::OneMinusSourceAlpha),
    blendFuncDstAlpha(types::BlendFunc::OneMinusSourceAlpha),
    blendEqRGB(types::BlendEquation::Add),
    blendEqAlpha(types::BlendEquation::Add),
    blendConstColor(),
    colorWriteState({true, true, true, true}),
    depthTestState(true),
    depthWriteState(true),
    blendState(false)
{

}




}

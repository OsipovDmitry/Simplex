#ifndef RENDERSTATE_P_H
#define RENDERSTATE_P_H

#include <array>

#include <glm/vec4.hpp>

#include <types/forwarddecl.h>

namespace scenegraph
{

class RenderStatePrivate
{
public:
    uint32_t layer;

    types::DepthTestFunc depthTestFunc;
    types::BlendFunc blendFuncSrcRGB, blendFuncDstRGB, blendFuncSrcAlpha, blendFuncDstAlpha;
    types::BlendEquation blendEqRGB, blendEqAlpha;
    glm::vec4 blendConstColor;

    std::array<bool, 4> colorWriteState;
    bool depthTestState;
    bool depthWriteState;
    bool blendState;

    RenderStatePrivate();


};

}

#endif // RENDERSTATE_P_H

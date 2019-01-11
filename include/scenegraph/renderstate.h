#ifndef RENDERSTATE_H
#define RENDERSTATE_H

#include <glm/vec4.hpp>

#include "scenegraph_global.h"
#include "forwarddecl.h"
#include "../types/renderstate.h"
#include "../utils/pimpl.h"
#include "../utils/noncopyble.h"
#include "../utils/customdeleter.h"

namespace scenegraph
{

class RenderStatePrivate;
class SCENEGRAPHSHARED_EXPORT RenderState {
    PIMPL(RenderState)
    NONCOPYBLE(RenderState)
    CUSTOMDELETER(RenderState)

public:
    void enableDepthTest(types::DepthTestFunc func = types::DepthTestFunc::Less);
    void disableDepthTest();
    bool depthTestState() const;
    types::DepthTestFunc depthTestFunc() const;

    void setDepthWriteState(bool state);
    bool depthWriteState() const;

    void setColorWriteState(bool r, bool g, bool b, bool a);
    void colorWriteState(bool& r, bool& g, bool& b, bool& a) const;

    void enableBlend(types::BlendFunc srcRGB = types::BlendFunc::SourceAlpha,
                     types::BlendFunc srcA = types::BlendFunc::SourceAlpha,
                     types::BlendFunc dstRGB = types::BlendFunc::OneMinusSourceAlpha,
                     types::BlendFunc dstA = types::BlendFunc::OneMinusSourceAlpha,
                     types::BlendEquation eqRGB = types::BlendEquation::Add,
                     types::BlendEquation eqA = types::BlendEquation::Add);
    void disableBlend();
    void setBlendConstColor(const glm::vec4& value);
    glm::vec4 blendConstColor() const;
    bool blendState() const;
    void blenFunc(types::BlendFunc& srcRGB, types::BlendFunc& srcA, types::BlendFunc& dstRGB, types::BlendFunc& dstA) const;
    void blendEquation(types::BlendEquation& eqRGB, types::BlendEquation& eqA) const;

    static RenderStatePtr create();

private:
    RenderState();
    ~RenderState();

    RenderStatePrivate *m_;
};

}

#endif // RENDERSTATE_H

#ifndef CONTEXT_H
#define CONTEXT_H

#include <glm/vec4.hpp>

#include "../utils/pimpl.h"
#include "../utils/noncopyble.h"
#include "../utils/customdeleter.h"
#include "../types/renderstate.h"
#include "renderer_global.h"
#include "forwarddecl.h"

namespace renderer {

class ContextPrivate;
class RENDERERSHARED_EXPORT Context {
    PIMPL(Context)
    NONCOPYBLE(Context)
    CUSTOMDELETER(Context)

public:
	WindowSurfacePtr windowSurface() const;
    ContextGroupPtr shareGroup() const;

	void bindUniformBuffer(BufferPtr buffer, uint32_t bindingPoint, int64_t size = -1, uint64_t offset = 0);
	void bindTexture(TexturePtr texture, int32_t slot);

    void enableDepthTest(types::DepthTestFunc func = types::DepthTestFunc::Less);
	void disableDepthTest();
	bool depthTestState() const;
    types::DepthTestFunc depthTestFunc() const;

	void setDepthWriteMask(bool state);
	bool depthWriteMask() const;

	void setColorWriteMask(bool r, bool g, bool b, bool a);
	void colorWriteMask(bool& r, bool& g, bool& b, bool& a) const;

    void enableBlend(types::BlendFunc srcRGB = types::BlendFunc::SourceAlpha,
                     types::BlendFunc srcA = types::BlendFunc::SourceAlpha,
                     types::BlendFunc dstRGB = types::BlendFunc::OneMinusSourceAlpha,
                     types::BlendFunc dstA = types::BlendFunc::OneMinusSourceAlpha,
                     types::BlendEquation eqRGB = types::BlendEquation::Add,
                     types::BlendEquation eqA = types::BlendEquation::Add);
    void disableBlend();
    void setBlendConstColor(const glm::vec4& value);
    bool blendState() const;
    void blenFunc(types::BlendFunc& srcRGB, types::BlendFunc& srcA, types::BlendFunc& dstRGB, types::BlendFunc& dstA) const;
    void blendEquation(types::BlendEquation& eqRGB, types::BlendEquation& eqA) const;
    glm::vec4 blendConstColor() const;

    static ContextPtr createContext(WindowSurfacePtr windowSurface, ContextGroupPtr shareGroup = nullptr); // Can also create context without surface(windowSurface == nullptr)
    static ContextPtr createContext(intptr_t windowId, int32_t r = 8, int32_t g = 8, int32_t b = 8, int32_t a = 8, int32_t d = 24, int32_t s = 8, ContextGroupPtr shareGroup = nullptr);

private:
    Context(WindowSurfacePtr windowSurface, ContextGroupPtr shareGroup = nullptr);
    ~Context();
	void init();

    ContextPrivate *m_;
};

}

#endif

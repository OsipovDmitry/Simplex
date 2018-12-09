#ifndef CONTEXT_H
#define CONTEXT_H

#include <glm/vec4.hpp>

#include "../utils/pimpl.h"
#include "../utils/noncopyble.h"
#include "../utils/enumclass.h"
#include "../utils/customdeleter.h"
#include "renderer_global.h"
#include "forwarddecl.h"

namespace renderer {

ENUMCLASS(DepthTestFunc, int32_t,
          LessEqual,
          GreaterEqual,
          Less,
          Greater,
          Equal,
          NotEqual,
          Newer,
          Always_)

ENUMCLASS(BlendFunc, int32_t,
          Zero,
          One,
          SourceColor,
          OneMinusSourceColor,
          DestColor,
          OneMinusDestColor,
          SourceAlpha,
          OneMinusSourceAlpha,
          DestAlpha,
          OneMinusDestAlpha,
          ConstColor,
          OneMinusConstColor,
          ConstAlpha,
          OneMinusConstAlpha,
          SourceAlphaSaturate)

ENUMCLASS(BlendEquation, int32_t,
          Add,
          Subtract,
          ReverseSubtract,
          Min,
          Max)

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

	void enableDepthTest(DepthTestFunc func = DepthTestFunc::Less);
	void disableDepthTest();
	bool depthTestState() const;
	DepthTestFunc depthTestFunc() const;

	void setDepthWriteMask(bool state);
	bool depthWriteMask() const;

	void setColorWriteMask(bool r, bool g, bool b, bool a);
	void colorWriteMask(bool& r, bool& g, bool& b, bool& a) const;

    void enableBlend(BlendFunc srcRGB = BlendFunc::SourceAlpha,
                     BlendFunc srcA = BlendFunc::SourceAlpha,
                     BlendFunc dstRGB = BlendFunc::OneMinusSourceAlpha,
                     BlendFunc dstA = BlendFunc::OneMinusSourceAlpha,
                     BlendEquation eqRGB = BlendEquation::Add,
                     BlendEquation eqA = BlendEquation::Add);
    void disableBlend();
    void setBlendConstColor(const glm::vec4& value);
    bool blendState() const;
    void blenFunc(BlendFunc& srcRGB, BlendFunc& srcA, BlendFunc& dstRGB, BlendFunc& dstA) const;
    void blendEquation(BlendEquation& eqRGB, BlendEquation& eqA) const;
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

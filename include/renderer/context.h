#ifndef CONTEXT_H
#define CONTEXT_H

#include "../utils/pimpl.h"
#include "../utils/enumclass.h"
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

class ContextPrivate;
class RENDERERSHARED_EXPORT Context : public std::enable_shared_from_this<Context> {
    PIMPL(Context)

public:
	~Context();

	WindowSurfacePtr windowSurface() const;
	ContextPtr sharedContext() const;

	ShaderPtr createShader(ShaderType type);
	ShaderPtr createSharedShader(ShaderPtr shader);

	ProgramPtr createProgram();
	ProgramPtr createSharedProgram(ProgramPtr program);

	BufferPtr createBuffer(BufferUsage usage, uint64_t size = 0, const void *pData = nullptr);
	BufferPtr createSharedBuffer(BufferPtr buffer);

	VertexArrayPtr createVertexArray();
	VertexArrayPtr createSharedVertexArray(VertexArrayPtr vertexArray);

	TexturePtr createTexture(TextureType type, TextureInternalFormat internalFormat, const TextureSize& size, int32_t numLevels = -1); // если numLevels == -1, то автоматически рассчитывается trunc(log2(size)) + 1
	TexturePtr createSharedTexture(TexturePtr texture);

	RenderbufferPtr createRenderbuffer(TextureInternalFormat internalFormat, uint32_t width, uint32_t height);
	FramebufferPtr createFramebuffer();
	FramebufferPtr mainFramebuffer();

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

	static ContextPtr createContext(WindowSurfacePtr windowSurface, ContextPtr sharedContext = nullptr); // Can also create context without surface(windowSurface == nullptr)
	static ContextPtr createContext(intptr_t windowId, int32_t r = 8, int32_t g = 8, int32_t b = 8, int32_t a = 8, int32_t d = 24, int32_t s = 8, ContextPtr sharedContext = nullptr);

private:
	Context(WindowSurfacePtr windowSurface, ContextPtr sharedContext = nullptr);
	void init();

    ContextPrivate *m_;
};

}

#endif

#ifndef CONTEXT_H
#define CONTEXT_H

#include <memory>
#include <vector>

#include "../utils/singletoon.h"
#include "renderer_global.h"

namespace renderer {

class DisplayPixelFormat;
using DisplayPixelFormatPtr = std::shared_ptr<DisplayPixelFormat>;
using DisplayPixelFormatsList = std::vector<DisplayPixelFormatPtr>;

class WindowSurface;
using WindowSurfacePtr = std::shared_ptr<WindowSurface>;

class Context;
using ContextPtr = std::shared_ptr<Context>;

class Shader;
using ShaderPtr = std::shared_ptr<Shader>;
enum class ShaderType : int32_t;

class Program;
using ProgramPtr = std::shared_ptr<Program>;

class Buffer;
using BufferPtr = std::shared_ptr<Buffer>;
enum class BufferUsage : int32_t;

class VertexArray;
using VertexArrayPtr = std::shared_ptr<VertexArray>;

class Texture;
using TexturePtr = std::shared_ptr<Texture>;
enum class TextureType : int32_t;
enum class TextureInternalFormat : int32_t;
struct TextureSize;

class Renderbuffer;
using RenderbufferPtr = std::shared_ptr<Renderbuffer>;

class Framebuffer;
using FramebufferPtr = std::shared_ptr<Framebuffer>;

class DisplayPrivate;
class RENDERERSHARED_EXPORT Display {
	SINGLETON(Display)
public:
	~Display();

	int32_t eglMinorVersion() const;
	int32_t eglMajorVersion() const;

	DisplayPixelFormatsList pixelFormatsList() const;
	DisplayPixelFormatPtr choosePixelFormat(int32_t r = 8, int32_t g = 8, int32_t b = 8, int32_t a = 8, int32_t d = 24, int32_t s = 8) const;

private:
	Display();

	DisplayPrivate *m;

	friend class DisplayPixelFormat;
	friend class WindowSurface;
	friend class ContextPrivate;
	friend class Context;
};

class DisplayPixelFormatPrivate;
class RENDERERSHARED_EXPORT DisplayPixelFormat {
public:
	~DisplayPixelFormat();

	int32_t redSize() const;
	int32_t greenSize() const;
	int32_t blueSize() const;
	int32_t alphaSize() const;
	int32_t depthSize() const;
	int32_t stencilSize() const;

private:
	DisplayPixelFormat();

	DisplayPixelFormatPrivate *m;

	friend class Display;
	friend class WindowSurface;
	friend class Context;
};

class WindowSurfacePrivate;
class RENDERERSHARED_EXPORT WindowSurface
{
public:
	~WindowSurface();

	DisplayPixelFormatPtr pixelFormat() const;
	void swapBuffers();

	static WindowSurfacePtr createWindowSurface(DisplayPixelFormatPtr pixelFormat, intptr_t windowID);

private:
	WindowSurface(DisplayPixelFormatPtr pixelFormat);

	WindowSurfacePrivate *m;

	friend class ContextPrivate;
	friend class Context;
};

enum class DepthTestFunc : int32_t {
	LessEqual,
	GreaterEqual,
	Less,
	Greater,
	Equal,
	NotEqual,
	Newer,
	Always_,
	Count
};
template <typename T> constexpr DepthTestFunc castToDepthTestFunc(T value) { return static_cast<DepthTestFunc>(value); }
template <typename T> constexpr T castFromDepthTestFunc(DepthTestFunc value) { return static_cast<T>(value); }

class ContextPrivate;
class RENDERERSHARED_EXPORT Context : public std::enable_shared_from_this<Context> {
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

	ContextPrivate *m;

	friend class Shader;
	friend class Program;
	friend class Buffer;
	friend class VertexArray;
	friend class Texture;
	friend class Renderbuffer;
	friend class Framebuffer;
};

}

#endif

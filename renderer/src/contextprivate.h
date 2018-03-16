#ifndef CONTEXTPRIVATE_H
#define CONTEXTPRIVATE_H

#include <memory>
#include <vector>

#include <EGL/egl.h>

namespace renderer {

class DisplayPixelFormat;
using DisplayPixelFormatPtr = std::shared_ptr<DisplayPixelFormat>;
using DisplayPixelFormatsList = std::vector<DisplayPixelFormatPtr>;

class WindowSurface;
using WindowSurfacePtr = std::shared_ptr<WindowSurface>;

class Context;
using ContextPtr = std::shared_ptr<Context>;
using ContextConstPtr = std::shared_ptr<const Context>;

class Program;
using ProgramPtr = std::shared_ptr<Program>;
using ProgramConstPtr = std::shared_ptr<const Program>;

class Buffer;
using BufferPtr = std::shared_ptr<Buffer>;
using BufferConstPtr = std::shared_ptr<const Buffer>;

class VertexArray;
using VertexArrayPtr = std::shared_ptr<VertexArray>;
using VertexArrayConstPtr = std::shared_ptr<const VertexArray>;

class Texture;
using TexturePtr = std::shared_ptr<Texture>;
using TextureConstPtr = std::shared_ptr<const Texture>;

class DisplayPrivate {
public:
	EGLDisplay display;
	EGLint majorVersion, minorVersion;
	DisplayPrivate() :
		display(nullptr),
		majorVersion(0),
		minorVersion(0)
	{}
};

class DisplayPixelFormatPrivate {
public:
	const Display *pDisplay;
	EGLConfig config;

	DisplayPixelFormatPrivate(const Display *pD) :
		pDisplay(pD),
		config(nullptr)
	{}
};

class WindowSurfacePrivate {
public:
	DisplayPixelFormatPtr pixelFormat;
	EGLSurface surface;

	WindowSurfacePrivate(DisplayPixelFormatPtr pf) :
		pixelFormat(pf),
		surface(nullptr)
	{}
};

enum class BufferTarget : int32_t {
	Array = 0,
	Element,
	CopyRead,
	CopyWrite,
	PixelPack,
	PixelUnpack,
	TransformFeedback,
	Uniform, // 32 slots for binding points
	Count = Uniform+32
};
template <typename T> constexpr BufferTarget castToBufferTarget(T value) { return static_cast<BufferTarget>(value); }
template <typename T> constexpr T castFromBufferTarget(BufferTarget value) { return static_cast<T>(value); }

class ContextPrivate {
public:
	Context *pParentContext;
	WindowSurfacePtr windowSurface;
	ContextPtr sharedContext;
	EGLContext context;

	static std::weak_ptr<const Context> currentContext;
	std::weak_ptr<const Program> currentProgram;
	std::array<std::weak_ptr<const Buffer>, castFromBufferTarget<size_t>(BufferTarget::Count)> currentBuffers;
	std::weak_ptr<const VertexArray> currentVertexArray;
	std::array<std::weak_ptr<const Texture>, 32> currentTextures;

	void bindThisContext() const;
	void bindProgram(ProgramConstPtr program);
	void bindBuffer(BufferConstPtr buffer, BufferTarget target, uint32_t bindingPoint = 0, int64_t size = -1, uint64_t offset = 0);
	void bindVertexArray(VertexArrayConstPtr vArray);
	int32_t bindTexture(TexturePtr texture, int32_t slot = -1); // если slot == -1, то пытаемся забиндить текстуру в любой свободный слот. Если свободного слота нет, то в 0.

	ContextPrivate(Context *pc, WindowSurfacePtr ws, ContextPtr sc);
};

}

#endif // CONTEXTPRIVATE_H

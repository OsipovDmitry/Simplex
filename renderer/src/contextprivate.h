#ifndef CONTEXTPRIVATE_H
#define CONTEXTPRIVATE_H

#include <memory>

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
	Array,
	Element,
	CopyRead,
	CopyWrite,
	PixelPack,
	PixelUnpack,
	TransformFeedback,
	Uniform,
	Count
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

	void bindThisContext() const;
	void bindProgram(ProgramConstPtr program);
	void bindBuffer(BufferTarget target, BufferConstPtr buffer);
	void bindVertexArray(VertexArrayConstPtr vArray);

	ContextPrivate(Context *pc, WindowSurfacePtr ws, ContextPtr sc);
};

}

#endif // CONTEXTPRIVATE_H

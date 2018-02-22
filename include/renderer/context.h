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


class DisplayPrivate;
class RENDERERSHARED_EXPORT Display {
	SINGLETON(Display)
public:
	~Display();

	int32_t eglMinorVersion() const;
	int32_t eglMajorVersion() const;

	DisplayPixelFormatsList pixelFormatsList() const;
	DisplayPixelFormatPtr choosePixelFormat(int32_t r = 8, int32_t g = 8, int32_t b = 8, int32_t a = 0, int32_t d = 24, int32_t s = 8) const;

private:
	Display();

	DisplayPrivate *m;

	friend class DisplayPixelFormat;
	friend class WindowSurface;
	friend class Context;
};

class DisplayPixelFormatPrivate;
class RENDERERSHARED_EXPORT DisplayPixelFormat {
public:
	~DisplayPixelFormat();

	const Display *display() const;

	int32_t redSize() const;
	int32_t greenSize() const;
	int32_t blueSize() const;
	int32_t alphaSize() const;
	int32_t depthSize() const;
	int32_t stencilSize() const;

private:
	DisplayPixelFormat(const Display *pDisplay);

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

	static WindowSurfacePtr createWindowSurface(DisplayPixelFormatPtr pixelFormat, intptr_t windowID);

private:
	WindowSurface(DisplayPixelFormatPtr pixelFormat);

	WindowSurfacePrivate *m;

	friend class Context;
};

class ContextPrivate;
class RENDERERSHARED_EXPORT Context : public std::enable_shared_from_this<Context> {
public:
	~Context();

	WindowSurfacePtr windowSurface() const;
	ContextPtr sharedContext() const;

	void swapBuffers();

	ShaderPtr createShader(ShaderType type);
	ProgramPtr createProgram();

	static ContextPtr createContext(WindowSurfacePtr windowSurface, ContextPtr sharedContext = nullptr);
	static ContextPtr createContext(intptr_t windowId, int32_t r = 8, int32_t g = 8, int32_t b = 8, int32_t a = 0, int32_t d = 24, int32_t s = 8, ContextPtr sharedContext = nullptr);
static void makeContextCurrent(const ContextPtr& context);
private:
	Context(WindowSurfacePtr windowSurface, ContextPtr sharedContext = nullptr);

	//static void makeContextCurrent(const ContextPtr& context);

	ContextPrivate *m;

	friend class Shader;
	friend class Program;
};

}

#endif

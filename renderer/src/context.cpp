#include <EGL/egl.h>

#include <renderer/context.h>
#include <renderer/shader.h>
#include <renderer/program.h>
#include <renderer/buffer.h>
#include <renderer/vertexarray.h>
#include <renderer/texture.h>
#include <renderer/renderbuffer.h>
#include <renderer/framebuffer.h>
#include <logger/logger.h>

#include "contextprivate.h"
#include "glutils.h"

namespace renderer {

Display::Display() :
	m(new DisplayPrivate)
{
	auto display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (display == EGL_NO_DISPLAY) {
		LOG_CRITICAL("Failed to get display");
		return;
	}

	EGLint majorVer, minorVer;
	if (eglInitialize(display, &majorVer, &minorVer) == EGL_FALSE) {
		auto error = eglGetError();
		if (error == EGL_BAD_DISPLAY)
			LOG_CRITICAL("Failed to initialize EGL for display: Invalid display");
		else if (error == EGL_NOT_INITIALIZED)
			LOG_CRITICAL("Failed to initialize EGL for display: EGL can not be initialized");
		return;
	}

	m->display = display;
	m->majorVersion = majorVer;
	m->minorVersion = minorVer;
}

Display::~Display()
{
	auto display = m->display;
	delete m;

	if (eglTerminate(display) == EGL_FALSE) {
		auto error = eglGetError();
		if (error == EGL_BAD_DISPLAY)
			LOG_CRITICAL("Failed to deinitialize EGL for display: Invalid display");
		return;
	}
}

int32_t Display::eglMinorVersion() const
{
	return m->minorVersion;
}

int32_t Display::eglMajorVersion() const
{
	return m->majorVersion;
}

DisplayPixelFormatsList Display::pixelFormatsList() const
{
	DisplayPixelFormatsList pixelFormatsList;

	EGLint numConfigs;
	if (eglGetConfigs(m->display, nullptr, 0, &numConfigs) == EGL_FALSE) {
		auto error = eglGetError();
		// ...
		LOG_ERROR("Can not get list of pixel formats");
		return pixelFormatsList;
	}

	std::vector<EGLConfig> configs(numConfigs, nullptr);
	eglGetConfigs(m->display, configs.data(), numConfigs, &numConfigs);
	for (auto config: configs) {
		if (!config)
			continue;
		DisplayPixelFormatPtr pixelFormatPtr(new DisplayPixelFormat());
		pixelFormatPtr->m->config = config;
		pixelFormatsList.push_back(pixelFormatPtr);
	}

	return pixelFormatsList;
}

DisplayPixelFormatPtr Display::choosePixelFormat(int32_t r, int32_t g, int32_t b, int32_t a, int32_t d, int32_t s) const
{
	EGLint attribs[] = {
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
		EGL_RED_SIZE, r,
		EGL_GREEN_SIZE, g,
		EGL_BLUE_SIZE, b,
		EGL_ALPHA_SIZE, a,
		EGL_DEPTH_SIZE, d,
		EGL_STENCIL_SIZE, s,
		EGL_NONE
	};
	EGLConfig config;
	EGLint numConfigs;
	if (eglChooseConfig(m->display, attribs, &config, 1, &numConfigs) == EGL_FALSE) {
		auto error = eglGetError();
		// ...
		LOG_ERROR("Can not get pixel format");
		return nullptr;
	}

	if (numConfigs == 0) {
		LOG_ERROR("Can not get pixel format");
		return nullptr;
	}

	DisplayPixelFormatPtr pixelFormatPtr(new DisplayPixelFormat());
	pixelFormatPtr->m->config = config;
	return pixelFormatPtr;
}

DisplayPixelFormat::DisplayPixelFormat() :
	m(new DisplayPixelFormatPrivate())
{
}

DisplayPixelFormat::~DisplayPixelFormat()
{
	delete m;
}

int32_t DisplayPixelFormat::redSize() const
{
	EGLint value = -1;
	eglGetConfigAttrib(Display::instance()->m->display, m->config, EGL_RED_SIZE, &value);
	return value;
}

int32_t DisplayPixelFormat::greenSize() const
{
	EGLint value = -1;
	eglGetConfigAttrib(Display::instance()->m->display, m->config, EGL_GREEN_SIZE, &value);
	return value;
}

int32_t DisplayPixelFormat::blueSize() const
{
	EGLint value = -1;
	eglGetConfigAttrib(Display::instance()->m->display, m->config, EGL_BLUE_SIZE, &value);
	return value;
}

int32_t DisplayPixelFormat::alphaSize() const
{
	EGLint value = -1;
	eglGetConfigAttrib(Display::instance()->m->display, m->config, EGL_ALPHA_SIZE, &value);
	return value;
}

int32_t DisplayPixelFormat::depthSize() const
{
	EGLint value = -1;
	eglGetConfigAttrib(Display::instance()->m->display, m->config, EGL_DEPTH_SIZE, &value);
	return value;
}

int32_t DisplayPixelFormat::stencilSize() const
{
	EGLint value = -1;
	eglGetConfigAttrib(Display::instance()->m->display, m->config, EGL_STENCIL_SIZE, &value);
	return value;
}

WindowSurface::~WindowSurface()
{
	auto surface = m->surface;
	auto pf = m->pixelFormat;
	delete m;

	if (eglDestroySurface(Display::instance()->m->display, surface) == EGL_FALSE) {
		auto error = eglGetError();
		// ...
		LOG_ERROR("Can not destory window surface");
		return;
	}
}

DisplayPixelFormatPtr WindowSurface::pixelFormat() const
{
	return m->pixelFormat;
}

void WindowSurface::swapBuffers()
{
	if (eglSwapBuffers(Display::instance()->m->display,
					   m->surface) == EGL_FALSE) {
		auto error = eglGetError();
		// ...
		LOG_ERROR("Can not swap color buffers");
		return;
	}
}

WindowSurfacePtr WindowSurface::createWindowSurface(DisplayPixelFormatPtr pixelFormat, intptr_t windowID)
{
	EGLint attribs[] = {
		EGL_NONE
	};

	auto surface = eglCreateWindowSurface(Display::instance()->m->display, pixelFormat->m->config, (EGLNativeWindowType)windowID, attribs);
	if (surface == EGL_NO_SURFACE) {
		auto error = eglGetError();
		// ...
		LOG_ERROR("Can not create window surface");
		return nullptr;
	}

	WindowSurfacePtr windowSurface(new WindowSurface(pixelFormat));
	windowSurface->m->surface = surface;

	return windowSurface;
}

WindowSurface::WindowSurface(DisplayPixelFormatPtr pixelFormat) :
	m(new WindowSurfacePrivate(pixelFormat))
{
}

Context::Context(WindowSurfacePtr windowSurface, ContextPtr sharedContext) :
	m(new ContextPrivate(this, windowSurface, sharedContext))
{
}

void Context::init()
{
	m->bindThisContext();
	CHECK_GL_ERROR(glPixelStorei(GL_UNPACK_ALIGNMENT, 1), "Can not initialize context");
}

Context::~Context()
{
	auto context = m->context;
	auto surface = m->windowSurface;
	delete m;

	if (eglDestroyContext(Display::instance()->m->display, context) == EGL_FALSE) {
		auto error = eglGetError();
		// ...
		LOG_ERROR("Can not destroy context");
		return;
	}
}

WindowSurfacePtr Context::windowSurface() const
{
	return m->windowSurface;
}

ContextPtr Context::sharedContext() const
{
	return m->sharedContext;
}

ShaderPtr Context::createShader(ShaderType type)
{
	return ShaderPtr(new Shader(shared_from_this(), type));
}

ShaderPtr Context::createSharedShader(ShaderPtr shader)
{
	return ShaderPtr(new Shader(shared_from_this(), shader));
}

ProgramPtr Context::createProgram()
{
	return ProgramPtr(new Program(shared_from_this()));
}

ProgramPtr Context::createSharedProgram(ProgramPtr program)
{
	return ProgramPtr(new Program(shared_from_this(), program));
}

BufferPtr Context::createBuffer(BufferUsage usage, uint64_t size, const void* pData)
{
	auto pBuffer = BufferPtr(new Buffer(shared_from_this()));
	pBuffer->init(usage, size, pData);
	return pBuffer;
}

BufferPtr Context::createSharedBuffer(BufferPtr buffer)
{
	return BufferPtr(new Buffer(shared_from_this(), buffer));
}

VertexArrayPtr Context::createVertexArray()
{
	return VertexArrayPtr(new VertexArray(shared_from_this()));
}

VertexArrayPtr Context::createSharedVertexArray(VertexArrayPtr vertexArray)
{
	auto pVertexArray = VertexArrayPtr(new VertexArray(shared_from_this())); // конструируем абсолютно новый VAO, так как OpenGL не умеет расшаривать VAO
	pVertexArray->initShared(vertexArray);
	return pVertexArray;
}

TexturePtr Context::createTexture(TextureType type, TextureInternalFormat internalFormat, const TextureSize& size, int32_t numLevels)
{
	auto pTexture = TexturePtr(new Texture(shared_from_this()));
	pTexture->init(type, numLevels, internalFormat, size);
	return pTexture;
}

TexturePtr Context::createSharedTexture(TexturePtr texture)
{
	return TexturePtr(new Texture(shared_from_this(), texture));
}

RenderbufferPtr Context::createRenderbuffer(TextureInternalFormat internalFormat, uint32_t width, uint32_t height)
{
	auto pRenderbuffer = RenderbufferPtr(new Renderbuffer(shared_from_this()));
	pRenderbuffer->init(internalFormat, width, height);
	return pRenderbuffer;
}

FramebufferPtr Context::createFramebuffer()
{
	return FramebufferPtr(new Framebuffer(shared_from_this(), std::false_type()));
}

FramebufferPtr Context::mainFramebuffer()
{
	return FramebufferPtr(new Framebuffer(shared_from_this(), std::true_type()));
}

void Context::bindUniformBuffer(BufferPtr buffer, uint32_t bindingPoint, int64_t size, uint64_t offset)
{
	m->bindThisContext();
	m->bindBuffer(buffer, BufferTarget::Uniform, bindingPoint, size, offset);
}

void Context::bindTexture(TexturePtr texture, int32_t slot)
{
	m->bindThisContext();
	m->bindTexture(texture, slot);
}

ContextPtr Context::createContext(WindowSurfacePtr windowSurface, ContextPtr sharedContext)
{
	static const EGLint attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 3,
		EGL_NONE
	};

	auto context = eglCreateContext(Display::instance()->m->display,
									windowSurface ? windowSurface->pixelFormat()->m->config : nullptr,
									sharedContext ? sharedContext->m->context : EGL_NO_CONTEXT,
									attribs);
	if (context == EGL_NO_CONTEXT) {
		auto error = eglGetError();
		// ...
		LOG_ERROR("Can not create context");
		return nullptr;
	}

	ContextPtr renderingContext(new Context(windowSurface, sharedContext));
	renderingContext->m->context = context;

	renderingContext->init();

	return renderingContext;
}

ContextPtr Context::createContext(intptr_t windowId, int32_t r, int32_t g, int32_t b, int32_t a, int32_t d, int32_t s, ContextPtr sharedContext)
{
	auto pDisplay = Display::instance();
	if (!pDisplay)
		return nullptr;

	auto pixelFormat = pDisplay->choosePixelFormat(r, g, b, a, d, s);
	if (!pixelFormat)
		return nullptr;

	auto surface = WindowSurface::createWindowSurface(pixelFormat, windowId);
	if (!surface)
		return nullptr;

	return createContext(surface, sharedContext);
}

}

#include <EGL/egl.h>

#include <renderer/display.h>
#include <renderer/context.h>
#include <renderer/contextgroup.h>
#include <renderer/shader.h>
#include <renderer/program.h>
#include <renderer/buffer.h>
#include <renderer/vertexarray.h>
#include <renderer/texture.h>
#include <renderer/renderbuffer.h>
#include <renderer/framebuffer.h>
#include <logger/logger.h>

#include "display_p.h"
#include "context_p.h"
#include "contextgroup_p.h"
#include "glutils.h"

namespace renderer {

Context::Context(WindowSurfacePtr windowSurface, ContextGroupPtr shareGroup) :
    m_(new ContextPrivate(this, windowSurface, shareGroup))
{
}

Context::~Context()
{
    auto context = m_->context;
    delete m_;

    if (eglDestroyContext(Display::instance().m()->display, context) == EGL_FALSE) {
        auto error = eglGetError();
        // ...
        LOG_ERROR("Can not destroy context");
        return;
    }
}

void Context::init()
{
    m_->bindThisContext();
	CHECK_GL_ERROR(glPixelStorei(GL_UNPACK_ALIGNMENT, 1), "Can not initialize context");
}

WindowSurfacePtr Context::windowSurface() const
{
    return m_->windowSurface;
}

ContextGroupPtr Context::shareGroup() const
{
    return m_->shareGroup;
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

void Context::enableDepthTest(DepthTestFunc func)
{
    m_->bindThisContext();
	CHECK_GL_ERROR(glEnable(GL_DEPTH_TEST), "Can not enable depth test");
	CHECK_GL_ERROR(glDepthFunc(toDepthTestGLFunc(func)), "Can not enable depth test");
}

void Context::disableDepthTest()
{
    m_->bindThisContext();
	CHECK_GL_ERROR(glDisable(GL_DEPTH_TEST), "Can not disable depth test");
}

bool Context::depthTestState() const
{
    m_->bindThisContext();
	GLboolean value;
	CHECK_GL_ERROR(glGetBooleanv(GL_DEPTH_TEST, &value), "Can not get depth test state", false);
	return value != GL_FALSE;
}

DepthTestFunc Context::depthTestFunc() const
{
    m_->bindThisContext();
	GLint value;
	CHECK_GL_ERROR(glGetIntegerv(GL_DEPTH_FUNC, &value), "Can not get depth test func", DepthTestFunc::Count);
	return fromDepthTestGLFunc(value);
}

void Context::setDepthWriteMask(bool state)
{
    m_->bindThisContext();
	CHECK_GL_ERROR(glDepthMask((GLboolean)state), "Can not set depth write mask");
}

bool Context::depthWriteMask() const
{
    m_->bindThisContext();
	GLboolean value;
	CHECK_GL_ERROR(glGetBooleanv(GL_DEPTH_WRITEMASK, &value), "Can not get depth write mask", false);
	return value != GL_FALSE;
}

void Context::setColorWriteMask(bool r, bool g, bool b, bool a)
{
    m_->bindThisContext();
	CHECK_GL_ERROR(glColorMask((GLboolean)r, (GLboolean)g, (GLboolean)b, (GLboolean)a), "Can not set color write mask");
}

void Context::colorWriteMask(bool& r, bool& g, bool& b, bool& a) const
{
    m_->bindThisContext();
	GLboolean value[4];
	CHECK_GL_ERROR(glGetBooleanv(GL_COLOR_WRITEMASK, value), "Can not get color write mask");
	r = value[0] != GL_FALSE;
	g = value[1] != GL_FALSE;
	b = value[2] != GL_FALSE;
	a = value[3] != GL_FALSE;
}

void Context::bindUniformBuffer(BufferPtr buffer, uint32_t bindingPoint, int64_t size, uint64_t offset)
{
    m_->bindThisContext();
    m_->bindBuffer(buffer.get(), BufferTarget::Uniform, bindingPoint, size, offset);
}

void Context::bindTexture(TexturePtr texture, int32_t slot)
{
    m_->bindThisContext();
    m_->bindTexture(texture.get(), slot);
}

ContextPtr Context::createContext(WindowSurfacePtr windowSurface, ContextGroupPtr shareGroup)
{
	static const EGLint attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 3,
		EGL_NONE
	};

    auto context = eglCreateContext(Display::instance().m()->display,
                                    windowSurface ? windowSurface->pixelFormat()->m()->config : nullptr,
                                    shareGroup ? shareGroup->contexts().back().lock()->m_->context : EGL_NO_CONTEXT,
									attribs);
	if (context == EGL_NO_CONTEXT) {
		auto error = eglGetError();
		// ...
		LOG_ERROR("Can not create context");
		return nullptr;
	}

    ContextPtr renderingContext(new Context(windowSurface, shareGroup ? shareGroup : ContextGroupPtr(new ContextGroup())), ContextDeleter());
    renderingContext->m_->context = context;
    renderingContext->m_->shareGroup->m()->add(renderingContext);

	renderingContext->init();

	return renderingContext;
}

ContextPtr Context::createContext(intptr_t windowId, int32_t r, int32_t g, int32_t b, int32_t a, int32_t d, int32_t s, ContextGroupPtr shareGroup)
{
    auto pixelFormat = DisplayPixelFormat::choosePixelFormat(r, g, b, a, d, s);
	if (!pixelFormat)
		return nullptr;

	auto surface = WindowSurface::createWindowSurface(pixelFormat, windowId);
	if (!surface)
		return nullptr;

    return createContext(surface, shareGroup);
}

}

#include <algorithm>

#include <logger/logger.h>
#include <renderer/context.h>
#include <renderer/program.h>
#include <renderer/buffer.h>
#include <renderer/vertexarray.h>
#include <renderer/texture.h>
#include <renderer/renderbuffer.h>
#include <renderer/framebuffer.h>

#include "glutils.h"
#include "contextprivate.h"
#include "programprivate.h"
#include "vertexarrayprivate.h"
#include "textureprivate.h"
#include "renderbufferprivate.h"
#include "framebufferprivate.h"

namespace renderer {

std::weak_ptr<const Context> ContextPrivate::currentContext;

ContextPrivate::ContextPrivate(Context* pc, WindowSurfacePtr ws, ContextPtr sc) :
	pPublicContext(pc),
	windowSurface(ws),
	sharedContext(sc),
	context(nullptr),
	currentProgram(),
	currentBuffers()
{}

void ContextPrivate::bindThisContext() const
{
	if (currentContext.lock().get() == pPublicContext)
		return;

	EGLSurface surface = windowSurface ? windowSurface->m->surface : nullptr;

	if (eglMakeCurrent(Display::instance()->m->display,
					   surface,
					   surface,
					   context) == EGL_FALSE) {
		auto error = eglGetError();
		// ...
		LOG_ERROR("Can not make context current");
		return;
	}

	currentContext = pPublicContext->shared_from_this();
}

void ContextPrivate::bindProgram(ProgramConstPtr program)
{
	if (currentProgram.lock() == program)
		return;

	CHECK_GL_ERROR(glUseProgram(program ? *program->m->id : 0), "Can not use program");
	currentProgram = program;
}

void ContextPrivate::bindBuffer(BufferConstPtr buffer, BufferTarget target, uint32_t bindingPoint, int64_t size, uint64_t offset)
{
	auto cacheIndex = castFromBufferTarget<size_t>(target) + bindingPoint;
	if (currentBuffers[cacheIndex].lock() == buffer)
		return;

	switch (target) {
		case BufferTarget::Uniform: {
			if (size == -1)
				size = buffer->size() - offset;
			CHECK_GL_ERROR(glBindBufferRange(toBufferGLTarget(target), bindingPoint, buffer ? *buffer->m->id : 0, offset, size), "Can not bind buffer");
			break;
		}
		default: {
			CHECK_GL_ERROR(glBindBuffer(toBufferGLTarget(target), buffer ? *buffer->m->id : 0), "Can not bind buffer");
			break;
		}
	}

	currentBuffers[cacheIndex] = buffer;
}

void ContextPrivate::bindVertexArray(VertexArrayConstPtr vArray)
{
	if (currentVertexArray.lock() == vArray)
		return;

	CHECK_GL_ERROR(glBindVertexArray(vArray ? *vArray->m->id : 0), "Can not bind VAO");
	currentVertexArray = vArray;
}

int32_t ContextPrivate::bindTexture(TextureConstPtr texture, int32_t slot)
{
	if (slot == -1) {
		auto iter = std::find_if(currentTextures.cbegin(), currentTextures.cend(), [](auto p) {
			return p.lock() == nullptr;
		});
		if (iter == currentTextures.cend())
			slot = 0;
		else
			slot = iter - currentTextures.cbegin();
	}

	if (currentTextures[slot].lock() == texture)
		return slot;

	auto GLtarget = texture ? toTextureGLType(texture->type()) : GL_TEXTURE_2D;
	auto GLid = texture ? *texture->m->id : 0;

	CHECK_GL_ERROR(glActiveTexture(GL_TEXTURE0 + slot), "Can not bind texture", slot);
	CHECK_GL_ERROR(glBindTexture(GLtarget, GLid), "Can not bind texture", slot);
	currentTextures[slot] = texture;

	return slot;
}

void ContextPrivate::bindRenderbuffer(RenderbufferConstPtr renderbuffer)
{
	if (currentRenderbuffer.lock() == renderbuffer)
		return;

	CHECK_GL_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer ? *renderbuffer->m->id : 0), "Can not bind renderbuffer");
	currentRenderbuffer = renderbuffer;
}

void ContextPrivate::bindFramebuffer(FramebufferConstPtr frambuffer)
{
	if (currentFramebuffer.lock() == frambuffer)
		return;


	if (frambuffer) {
		CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, *frambuffer->m->id), "Can not bind frambuffer");
		CHECK_GL_ERROR(glViewport(frambuffer->m->viewport.x, frambuffer->m->viewport.y, frambuffer->m->viewport.z, frambuffer->m->viewport.w), "Can not bind frambuffer");
	}
	else {
		CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, 0), "Can not bind frambuffer");
	}
	currentFramebuffer = frambuffer;
}

}

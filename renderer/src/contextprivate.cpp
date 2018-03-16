#include <algorithm>

#include <logger/logger.h>
#include <renderer/context.h>
#include <renderer/program.h>
#include <renderer/buffer.h>
#include <renderer/vertexarray.h>
#include <renderer/texture.h>

#include "glutils.h"
#include "contextprivate.h"
#include "programprivate.h"
#include "bufferprivate.h"
#include "vertexarrayprivate.h"
#include "textureprivate.h"

namespace renderer {

namespace {

const std::array<GLenum, castFromBufferTarget<size_t>(BufferTarget::Count)> bufferTargetTable {
	GL_ARRAY_BUFFER,
	GL_ELEMENT_ARRAY_BUFFER,
	GL_COPY_READ_BUFFER,
	GL_COPY_WRITE_BUFFER,
	GL_PIXEL_PACK_BUFFER,
	GL_PIXEL_UNPACK_BUFFER,
	GL_TRANSFORM_FEEDBACK_BUFFER,
	GL_UNIFORM_BUFFER
};

GLenum toBufferGLTarget(BufferTarget val) {
	return bufferTargetTable[castFromBufferTarget<size_t>(val)];
}

BufferTarget fromBufferGLTarget(GLenum val) {
	return castToBufferTarget(std::find(bufferTargetTable.cbegin(), bufferTargetTable.cend(), val) - bufferTargetTable.cbegin());
}

}

std::weak_ptr<const Context> ContextPrivate::currentContext;

ContextPrivate::ContextPrivate(Context* pc, WindowSurfacePtr ws, ContextPtr sc) :
	pParentContext(pc),
	windowSurface(ws),
	sharedContext(sc),
	context(nullptr),
	currentProgram(),
	currentBuffers()
{}

void ContextPrivate::bindThisContext() const
{
	if (currentContext.lock().get() == pParentContext)
		return;

	if (eglMakeCurrent(windowSurface->pixelFormat()->display()->m->display,
					   windowSurface->m->surface,
					   windowSurface->m->surface,
					   context) == EGL_FALSE) {
		auto error = eglGetError();
		// ...
		LOG_ERROR("Can not make context current");
		return;
	}

	currentContext = pParentContext->shared_from_this();
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

int32_t ContextPrivate::bindTexture(TexturePtr texture, int32_t slot)
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

	CHECK_GL_ERROR(glBindTexture(GLtarget, GLid), "Can not bind texture", slot);
	currentTextures[slot] = texture;

	return slot;
}

}

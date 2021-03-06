#include <algorithm>

#include <logger/logger.h>
#include <renderer/display.h>
#include <renderer/context.h>
#include <renderer/program.h>
#include <renderer/buffer.h>
#include <renderer/vertexarray.h>
#include <renderer/texture.h>
#include <renderer/renderbuffer.h>
#include <renderer/framebuffer.h>

#include "glutils.h"
#include "context_p.h"
#include "display_p.h"
#include "program_p.h"
#include "vertexarray_p.h"
#include "texture_p.h"
#include "renderbuffer_p.h"
#include "framebuffer_p.h"

namespace renderer {

const Context *ContextPrivate::pCurrentContext;

ContextPrivate::ContextPrivate(Context* pc, WindowSurfacePtr ws, ContextGroupPtr sg) :
	pPublicContext(pc),
	windowSurface(ws),
    shareGroup(sg),
	context(nullptr),
    pCurrentProgram(),
    pCurrentBuffers(),
    pCurrentVertexArray(),
    pCurrentTextures(),
    pCurrentRenderbuffer(),
    pCurrentFramebuffer()
{}

void ContextPrivate::bindThisContext() const
{
    if (pCurrentContext == pPublicContext)
        return;

    EGLSurface surface = windowSurface ? windowSurface->m()->surface : nullptr;

    if (eglMakeCurrent(Display::instance().m()->display,
                       surface,
                       surface,
                       context) == EGL_FALSE) {
        auto error = eglGetError();
        // ...
        LOG_ERROR("Can not make context current");
        return;
    }

    pCurrentContext = pPublicContext;

    static int idx = 0;
    LOG_DEBUG(std::to_string(++idx));
}

void ContextPrivate::bindProgram(const Program *program)
{
    if (pCurrentProgram == program)
		return;

    CHECK_GL_ERROR(glUseProgram(program ? program->m()->id : 0), "Can not use program");
    pCurrentProgram = program;
}

void ContextPrivate::bindBuffer(const Buffer *buffer, BufferTarget target, uint32_t bindingPoint, int64_t size, uint64_t offset)
{
    auto cacheIndex = castFromBufferTarget(target) + bindingPoint;
    if (pCurrentBuffers[cacheIndex] == buffer)
		return;

	switch (target) {
		case BufferTarget::Uniform: {
			if (size == -1)
				size = buffer->size() - offset;
            CHECK_GL_ERROR(glBindBufferRange(toBufferGLTarget(target), bindingPoint, buffer ? buffer->m()->id : 0, offset, size), "Can not bind buffer");
			break;
		}
		default: {
            CHECK_GL_ERROR(glBindBuffer(toBufferGLTarget(target), buffer ? buffer->m()->id : 0), "Can not bind buffer");
			break;
		}
	}

    pCurrentBuffers[cacheIndex] = buffer;
}

void ContextPrivate::bindVertexArray(const VertexArray *vArray)
{
    if (pCurrentVertexArray == vArray)
        return;

    CHECK_GL_ERROR(glBindVertexArray(vArray ? vArray->m()->id : 0), "Can not bind VAO");
    pCurrentVertexArray = vArray;
}

int32_t ContextPrivate::bindTexture(const Texture *texture, int32_t slot)
{
	if (slot == -1) {
        auto iter = std::find(pCurrentTextures.cbegin(), pCurrentTextures.cend(), nullptr);
        if (iter == pCurrentTextures.cend())
			slot = 0;
		else
            slot = std::distance(pCurrentTextures.cbegin(), iter);
	}

    if (pCurrentTextures[slot] == texture)
		return slot;

	auto GLtarget = texture ? toTextureGLType(texture->type()) : GL_TEXTURE_2D;
    auto GLid = texture ? texture->m()->id : 0;

	CHECK_GL_ERROR(glActiveTexture(GL_TEXTURE0 + slot), "Can not bind texture", slot);
	CHECK_GL_ERROR(glBindTexture(GLtarget, GLid), "Can not bind texture", slot);
    pCurrentTextures[slot] = texture;

    return slot;
}

void ContextPrivate::unbindTextureIfCurrent(const Texture *texture)
{
    auto iter = std::find(pCurrentTextures.cbegin(), pCurrentTextures.cend(), texture);
    if (iter != pCurrentTextures.cend()) {
        bindTexture(nullptr, std::distance(pCurrentTextures.cbegin(), iter));
    }
}

void ContextPrivate::bindRenderbuffer(const Renderbuffer *renderbuffer)
{
    if (pCurrentRenderbuffer == renderbuffer)
		return;

    CHECK_GL_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer ? renderbuffer->m()->id : 0), "Can not bind renderbuffer");
    pCurrentRenderbuffer = renderbuffer;
}

void ContextPrivate::bindFramebuffer(const Framebuffer *frambuffer)
{
    if (pCurrentFramebuffer == frambuffer)
        return;

	if (frambuffer) {
        CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, frambuffer->m()->id), "Can not bind frambuffer");
        const auto& vp = frambuffer->m()->viewport;
        CHECK_GL_ERROR(glViewport(vp.x, vp.y, vp.z, vp.w), "Can not bind frambuffer");
	}
	else {
		CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, 0), "Can not bind frambuffer");
	}
	pCurrentFramebuffer = frambuffer;
}

const std::array<GLenum, types::castFromDepthTestFunc(types::DepthTestFunc::Count)> depthTestFuncTable {
	GL_LEQUAL,
	GL_GEQUAL,
	GL_LESS,
	GL_GREATER,
	GL_EQUAL,
	GL_NOTEQUAL,
	GL_ALWAYS,
	GL_NEVER
};

GLenum toDepthTestGLFunc(types::DepthTestFunc val)
{
    return depthTestFuncTable[types::castFromDepthTestFunc(val)];
}

types::DepthTestFunc fromDepthTestGLFunc(GLenum val)
{
    return types::castToDepthTestFunc(std::find(depthTestFuncTable.cbegin(), depthTestFuncTable.cend(), val) - depthTestFuncTable.cbegin());
}


const std::array<GLenum, types::castFromBlendFunc(types::BlendFunc::Count)> blendFuncTable {
    GL_ZERO,
    GL_ONE,
    GL_SRC_COLOR,
    GL_ONE_MINUS_SRC_COLOR,
    GL_DST_COLOR,
    GL_ONE_MINUS_DST_COLOR,
    GL_SRC_ALPHA,
    GL_ONE_MINUS_SRC_ALPHA,
    GL_DST_ALPHA,
    GL_ONE_MINUS_DST_ALPHA,
    GL_CONSTANT_COLOR,
    GL_ONE_MINUS_CONSTANT_COLOR,
    GL_CONSTANT_ALPHA,
    GL_ONE_MINUS_CONSTANT_ALPHA,
    GL_SRC_ALPHA_SATURATE
};

GLenum toBlendGLFunc(types::BlendFunc val)
{
    return blendFuncTable[types::castFromBlendFunc(val)];
}

types::BlendFunc fromBlendGLFunc(GLenum val)
{
    return types::castToBlendFunc(std::find(blendFuncTable.cbegin(), blendFuncTable.cend(), val) - blendFuncTable.cbegin());
}

const std::array<GLenum, types::castFromBlendEquation(types::BlendEquation::Count)> blendEquationTable {
    GL_FUNC_ADD,
    GL_FUNC_SUBTRACT,
    GL_FUNC_REVERSE_SUBTRACT,
    GL_MIN,
    GL_MAX
};

GLenum toBlendGLEquation(types::BlendEquation val)
{
    return blendEquationTable[types::castFromBlendEquation(val)];
}

types::BlendEquation fromBlendGLEquation(GLenum val)
{
    return types::castToBlendEquation(std::find(blendEquationTable.cbegin(), blendEquationTable.cend(), val) - blendEquationTable.cbegin());
}

}

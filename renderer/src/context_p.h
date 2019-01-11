#ifndef CONTEXTPRIVATE_H
#define CONTEXTPRIVATE_H

#include <memory>

#include <EGL/egl.h>

#include <types/renderstate.h>
#include <renderer/forwarddecl.h>

#include "buffer_p.h"

namespace renderer {

GLenum toDepthTestGLFunc(types::DepthTestFunc val);
types::DepthTestFunc fromDepthTestGLFunc(GLenum val);

GLenum toBlendGLFunc(types::BlendFunc val);
types::BlendFunc fromBlendGLFunc(GLenum val);

GLenum toBlendGLEquation(types::BlendEquation val);
types::BlendEquation fromBlendGLEquation(GLenum val);

class ContextPrivate {
public:
	Context *pPublicContext;
	WindowSurfacePtr windowSurface;
    ContextGroupPtr shareGroup;
	EGLContext context;

    static const Context *pCurrentContext;
    const Program *pCurrentProgram;
    std::array<const Buffer*, castFromBufferTarget(BufferTarget::Count)> pCurrentBuffers;
    const VertexArray *pCurrentVertexArray;
    std::array<const Texture*, 32> pCurrentTextures;
    const Renderbuffer *pCurrentRenderbuffer;
    const Framebuffer *pCurrentFramebuffer;

    void bindThisContext() const;
    void bindProgram(const Program *program);
    void bindBuffer(const Buffer *buffer, BufferTarget target, uint32_t bindingPoint = 0, int64_t size = -1, uint64_t offset = 0);
    void bindVertexArray(const VertexArray *vArray);
    int32_t bindTexture(const Texture *texture, int32_t slot = -1); // если slot == -1, то пытаемся забиндить текстуру в любой свободный слот. Если свободного слота нет, то в 0.
    void unbindTextureIfCurrent(const Texture *texture);
    void bindRenderbuffer(const Renderbuffer *renderbuffer);
    void bindFramebuffer(const Framebuffer *frambuffer);

    ContextPrivate(Context *pc, WindowSurfacePtr ws, ContextGroupPtr sg);
};

}

#endif // CONTEXTPRIVATE_H

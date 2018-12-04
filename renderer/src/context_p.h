#ifndef CONTEXTPRIVATE_H
#define CONTEXTPRIVATE_H

#include <memory>

#include <EGL/egl.h>

#include <renderer/forwarddecl.h>

#include "buffer_p.h"

namespace renderer {

enum class DepthTestFunc : int32_t;
GLenum toDepthTestGLFunc(DepthTestFunc val);
DepthTestFunc fromDepthTestGLFunc(GLenum val);

class ContextPrivate {
public:
	Context *pPublicContext;
	WindowSurfacePtr windowSurface;
    ContextGroupPtr shareGroup;
	EGLContext context;

    static const Context *pCurrentContext;
    const Program *pCurrentProgram;
    std::array<const Buffer*, castFromBufferTarget(BufferTarget::Count)> pCurrentBuffers;
	std::weak_ptr<const VertexArray> currentVertexArray;
    std::array<const Texture*, 32> pCurrentTextures;
	std::weak_ptr<const Renderbuffer> currentRenderbuffer;
	std::weak_ptr<const Framebuffer> currentFramebuffer;

	void bindThisContext() const;
    void bindProgram(const Program *program);
    void bindBuffer(const Buffer *buffer, BufferTarget target, uint32_t bindingPoint = 0, int64_t size = -1, uint64_t offset = 0);
	void bindVertexArray(VertexArrayConstPtr vArray);
    int32_t bindTexture(const Texture *texture, int32_t slot = -1); // если slot == -1, то пытаемся забиндить текстуру в любой свободный слот. Если свободного слота нет, то в 0.
    void unbindTextureIfCurrent(const Texture *texture);
    void bindRenderbuffer(RenderbufferConstPtr renderbuffer);
	void bindFramebuffer(FramebufferConstPtr frambuffer);

    ContextPrivate(Context *pc, WindowSurfacePtr ws, ContextGroupPtr sg);
};

}

#endif // CONTEXTPRIVATE_H

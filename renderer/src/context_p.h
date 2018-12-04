#ifndef CONTEXTPRIVATE_H
#define CONTEXTPRIVATE_H

#include <memory>

#include <EGL/egl.h>

#include <renderer/forwarddecl.h>

#include "bufferprivate.h"

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

	static std::weak_ptr<const Context> currentContext;
	std::weak_ptr<const Program> currentProgram;
	std::array<std::weak_ptr<const Buffer>, castFromBufferTarget<size_t>(BufferTarget::Count)> currentBuffers;
	std::weak_ptr<const VertexArray> currentVertexArray;
	std::array<std::weak_ptr<const Texture>, 32> currentTextures;
	std::weak_ptr<const Renderbuffer> currentRenderbuffer;
	std::weak_ptr<const Framebuffer> currentFramebuffer;

	void bindThisContext() const;
	void bindProgram(ProgramConstPtr program);
	void bindBuffer(BufferConstPtr buffer, BufferTarget target, uint32_t bindingPoint = 0, int64_t size = -1, uint64_t offset = 0);
	void bindVertexArray(VertexArrayConstPtr vArray);
	int32_t bindTexture(TextureConstPtr texture, int32_t slot = -1); // если slot == -1, то пытаемся забиндить текстуру в любой свободный слот. Если свободного слота нет, то в 0.
	void bindRenderbuffer(RenderbufferConstPtr renderbuffer);
	void bindFramebuffer(FramebufferConstPtr frambuffer);

    ContextPrivate(Context *pc, WindowSurfacePtr ws, ContextGroupPtr sg);
};

}

#endif // CONTEXTPRIVATE_H

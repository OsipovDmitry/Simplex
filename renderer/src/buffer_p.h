#ifndef BUFFERPRIVATE_H
#define BUFFERPRIVATE_H

#include <GLES3/gl3.h>

#include <utils/enumclass.h>
#include <renderer/forwarddecl.h>

namespace renderer {

class BufferPrivate {
public:
	ContextPtr context;
    GLuint id;

    BufferPrivate(ContextPtr context_) :
		context(context_),
        id(0)
	{}

};

ENUMCLASS(BufferTarget, int32_t,
          Array,
          Element,
          CopyRead,
          CopyWrite,
          PixelPack,
          PixelUnpack,
          TransformFeedback,
          Uniform // 32 slots for binding points
)

GLenum toBufferGLTarget(BufferTarget val);
BufferTarget fromBufferGLTarget(GLenum val);

}

#endif // BUFFERPRIVATE_H

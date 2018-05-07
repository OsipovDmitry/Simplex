#include <algorithm>

#include "bufferprivate.h"

namespace renderer {

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

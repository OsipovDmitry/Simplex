#ifndef FRAMEBUFFERPRIVATE_H
#define FRAMEBUFFERPRIVATE_H

#include <array>

#include <GLES3/gl3.h>
#include <glm/vec4.hpp>

#include <renderer/forwarddecl.h>
#include <renderer/framebuffer.h>

namespace renderer {

GLenum toFramebufferGLAttachment(FramebufferAttachment val);
FramebufferAttachment fromFramebufferGLAttachment(GLenum val);

GLenum toPrimitiveGLType(PrimitiveType val);
PrimitiveType fromPrimitiveGLType(GLenum val);

GLenum toGeometryIndexGLType(GeometryIndexType val);
GeometryIndexType fromGeometryIndexGLType(GLenum val);

class FramebufferPrivate {
public:
	glm::uvec4 viewport;
	ContextPtr context;
    GLuint id;
    std::array<RenderbufferPtr, castFromFramebufferAttachment(FramebufferAttachment::Count)> renderbuffers;
    std::array<TexturePtr, castFromFramebufferAttachment(FramebufferAttachment::Count)> textures;

    FramebufferPrivate(ContextPtr context_) :
		viewport(),
		context(context_),
        id(0),
		renderbuffers(),
		textures()
	{}
};

}

#endif // FRAMEBUFFERPRIVATE_H

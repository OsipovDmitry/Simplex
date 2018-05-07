#ifndef FRAMEBUFFERPRIVATE_H
#define FRAMEBUFFERPRIVATE_H

#include <array>

#include <renderer/framebuffer.h>
#include <renderer/texture.h>
#include <renderer/renderbuffer.h>

#include "glutils.h"

namespace renderer {

GLenum toFramebufferGLAttachment(FramebufferAttachment val);
FramebufferAttachment fromFramebufferGLAttachment(GLenum val);

GLenum toPrimitiveGLType(PrimitiveType val);
PrimitiveType fromPrimitiveGLType(GLenum val);

GLenum toGeometryIndexGLType(GeometryIndexType val);
GeometryIndexType fromGeometryIndexGLType(GLenum val);

GLenum toDepthTestGLFunc(DepthTestFunc val);
DepthTestFunc fromDepthTestGLFunc(GLenum val);

class Context;
using ContextPtr = std::shared_ptr<Context>;

class Texture;
using TexturePtr = std::shared_ptr<Texture>;

class Renderbuffer;
using RenderbufferPtr = std::shared_ptr<Renderbuffer>;

class FramebufferPrivate {
public:
	glm::uvec4 viewport;
	DepthTestFunc depthTestFunc;
	bool depthTestState;
	ContextPtr context;
	GLuintPtr id;
	std::array<RenderbufferPtr, castFromFramebufferAttachment<size_t>(FramebufferAttachment::Count)> renderbuffers;
	std::array<TexturePtr, castFromFramebufferAttachment<size_t>(FramebufferAttachment::Count)> textures;

	FramebufferPrivate(ContextPtr context_, GLuintPtr id_) :
		viewport(),
		depthTestFunc(DepthTestFunc::Less),
		depthTestState(false),
		context(context_),
		id(id_),
		renderbuffers(),
		textures()
	{}
};

}

#endif // FRAMEBUFFERPRIVATE_H

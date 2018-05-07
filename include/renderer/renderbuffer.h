#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include <memory>
#include <string>

#include "renderer_global.h"

namespace renderer {

class Context;
using ContextPtr = std::shared_ptr<Context>;

class Renderbuffer;
using RenderbufferPtr = std::shared_ptr<Renderbuffer>;

enum class TextureInternalFormat : int32_t;

class RenderbufferPrivate;
class RENDERERSHARED_EXPORT Renderbuffer : public std::enable_shared_from_this<Renderbuffer> {
public:
	~Renderbuffer();

	ContextPtr context() const;
	TextureInternalFormat internalFormat() const;
	uint32_t width();
	uint32_t height();

private:
	Renderbuffer(ContextPtr context);
	void init(TextureInternalFormat internalFormat, uint32_t width, uint32_t height);

	RenderbufferPrivate *m;

	friend class Context;
	friend class ContextPrivate;
	friend class Framebuffer;
};

}

#endif // RENDERBUFFER_H

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <memory>

#include <glm/vec4.hpp>

#include "renderer_global.h"
#include "forwarddecl.h"
#include "../utils/pimpl.h"
#include "../utils/noncopyble.h"
#include "../utils/customdeleter.h"
#include "../utils/enumclass.h"
#include "../types/forwarddecl.h"

namespace renderer {

ENUMCLASS(FramebufferAttachment, int32_t,
    Color0,
	Color1,
	Color2,
	Color3,
	Color4,
	Color5,
	Color6,
	Color7,
	Depth,
	Stencil,
    DepthStencil
)

class FramebufferPrivate;
class RENDERERSHARED_EXPORT Framebuffer {
    PIMPL(Framebuffer)
    NONCOPYBLE(Framebuffer)
    CUSTOMDELETER(Framebuffer)

public:
	ContextPtr context() const;

	void attachBuffer(FramebufferAttachment attachment, RenderbufferPtr renderbuffer);
	void attachBuffer(FramebufferAttachment attachment, TexturePtr texture, uint32_t level = 0); // 2d
	void attachBuffer(FramebufferAttachment attachment, TexturePtr texture, TextureCubemapSide side, uint32_t level = 0); // cubemap
	void attachBuffer(FramebufferAttachment attachment, TexturePtr texture, uint32_t layer, uint32_t level = 0); // 2d-array, 3d

	void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
	glm::uvec4 viewport() const;

	void clearColorBuffer(uint32_t index, const glm::vec4& value);
	void clearColorBuffer(uint32_t index, const glm::uvec4& value);
	void clearColorBuffer(uint32_t index, const glm::ivec4& value);
	void clearDepthBuffer(float value);
	void clearStencilBuffer(int32_t value);
	void clearDepthStencilBuffer(float depth, int32_t stencil);

	void copyColorBuffer(const glm::uvec4& dstRect, const glm::uvec4& srcRect, FramebufferPtr srcBuffer, uint32_t dstColorIndex = 0, uint32_t srcColorIndex = 0);
	void copyDepthBuffer(const glm::uvec4& dstRect, const glm::uvec4& srcRect, FramebufferPtr srcBuffer);
	void copyStencilBuffer(const glm::uvec4& dstRect, const glm::uvec4& srcRect, FramebufferPtr srcBuffer);

    void renderIndexedGeometry(ProgramPtr program, VertexArrayPtr vertexArray, types::PrimitiveType primitiveType, uint32_t numIndices, types::IndexType indicesType, const void *pIndices);
    void renderIndexedGeometry(ProgramPtr program, VertexArrayPtr vertexArray, types::PrimitiveType primitiveType, uint32_t numIndices, types::IndexType indicesType, uint32_t bufferOffset = 0);

    static FramebufferPtr create(ContextPtr context);
    static FramebufferPtr mainFramebuffer(ContextPtr context);

private:
	Framebuffer(ContextPtr context, std::false_type);
	Framebuffer(ContextPtr context, std::true_type); // main framebuffer (id == 0)
    ~Framebuffer();

    FramebufferPrivate *m_;
};

}

#endif // FRAMEBUFFER_H

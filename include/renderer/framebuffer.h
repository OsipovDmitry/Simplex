#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <memory>

#include <glm/vec4.hpp>

#include "renderer_global.h"

namespace renderer {

class Context;
using ContextPtr = std::shared_ptr<Context>;

class Texture;
using TexturePtr = std::shared_ptr<Texture>;
enum class TextureCubemapSide : int32_t;

class Program;
using ProgramPtr = std::shared_ptr<Program>;

class VertexArray;
using VertexArrayPtr = std::shared_ptr<VertexArray>;

class Renderbuffer;
using RenderbufferPtr = std::shared_ptr<Renderbuffer>;

class Framebuffer;
using FramebufferPtr = std::shared_ptr<Framebuffer>;

enum class FramebufferAttachment : int32_t {
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
	DepthStencil,
	Count,
};
template <typename T> constexpr FramebufferAttachment castToFramebufferAttachment(T value) { return static_cast<FramebufferAttachment>(value); }
template <typename T> constexpr T castFromFramebufferAttachment(FramebufferAttachment value) { return static_cast<T>(value); }

enum class PrimitiveType : int32_t {
	Points,
	LineStrip,
	LineLoop,
	Lines,
	TriangleStrip,
	TriangleFan,
	Triangles,
	Count
};
template <typename T> constexpr PrimitiveType castToPrimitiveType(T value) { return static_cast<PrimitiveType>(value); }
template <typename T> constexpr T castFromPrimitiveType(PrimitiveType value) { return static_cast<T>(value); }

enum class GeometryIndexType : int32_t {
	Type_8ui,
	Type_16ui,
	Type_32ui,
	Count
};
template <typename T> constexpr GeometryIndexType castToGeometryIndexType(T value) { return static_cast<GeometryIndexType>(value); }
template <typename T> constexpr T castFromGeometryIndexType(GeometryIndexType value) { return static_cast<T>(value); }

enum class DepthTestFunc : int32_t {
	LessEqual,
	GreaterEqual,
	Less,
	Greater,
	Equal,
	NotEqual,
	Newer,
	Always_,
	Count
};
template <typename T> constexpr DepthTestFunc castToDepthTestFunc(T value) { return static_cast<DepthTestFunc>(value); }
template <typename T> constexpr T castFromDepthTestFunc(DepthTestFunc value) { return static_cast<T>(value); }

class FramebufferPrivate;
class RENDERERSHARED_EXPORT Framebuffer : public std::enable_shared_from_this<Framebuffer> {
public:
	~Framebuffer();

	ContextPtr context() const;

	void attachBuffer(FramebufferAttachment attachment, RenderbufferPtr renderbuffer);
	void attachBuffer(FramebufferAttachment attachment, TexturePtr texture, uint32_t level = 0); // 2d
	void attachBuffer(FramebufferAttachment attachment, TexturePtr texture, TextureCubemapSide side, uint32_t level = 0); // cubemap
	void attachBuffer(FramebufferAttachment attachment, TexturePtr texture, uint32_t layer, uint32_t level = 0); // 2d-array, 3d

	void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
	glm::uvec4 viewport() const;

	void enableDepthTest(DepthTestFunc func = DepthTestFunc::Less);
	void disableDepthTest();
	bool depthTestState() const;
	DepthTestFunc depthTestFunc() const;

	void clearColorBuffer(uint32_t index, const glm::vec4& value);
	void clearColorBuffer(uint32_t index, const glm::uvec4& value);
	void clearColorBuffer(uint32_t index, const glm::ivec4& value);
	void clearDepthBuffer(float value);
	void clearStencilBuffer(int32_t value);
	void clearDepthStencilBuffer(float depth, int32_t stencil);

	void copyColorBuffer(const glm::uvec4& dstRect, const glm::uvec4& srcRect, FramebufferPtr srcBuffer, uint32_t dstColorIndex = 0, uint32_t srcColorIndex = 0);
	void copyDepthBuffer(const glm::uvec4& dstRect, const glm::uvec4& srcRect, FramebufferPtr srcBuffer);
	void copyStencilBuffer(const glm::uvec4& dstRect, const glm::uvec4& srcRect, FramebufferPtr srcBuffer);

	void renderIndexedGeometry(ProgramPtr program, VertexArrayPtr vertexArray, PrimitiveType primitiveType, uint32_t numIndices, GeometryIndexType indicesType, const void *pIndices);
	void renderIndexedGeometry(ProgramPtr program, VertexArrayPtr vertexArray, PrimitiveType primitiveType, uint32_t numIndices, GeometryIndexType indicesType, uint32_t bufferOffset = 0);

private:
	Framebuffer(ContextPtr context, std::false_type);
	Framebuffer(ContextPtr context, std::true_type); // main framebuffer (id == 0)

	FramebufferPrivate *m;

	friend class Context;
	friend class ContextPrivate;
};

}

#endif // FRAMEBUFFER_H

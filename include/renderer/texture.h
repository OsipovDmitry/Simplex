#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>

#include "renderer_global.h"

namespace types {

enum class ImageFormat : int32_t;
enum class ImageType : int32_t;

}

namespace renderer {

class Context;
using ContextPtr = std::shared_ptr<Context>;

class Texture;
using TexturePtr = std::shared_ptr<Texture>;

enum class TextureType : int32_t {
	Type_2D,
	Type_3D,
	Type_2DArray,
	Type_CubeMap,
	Count
};
template <typename T> constexpr TextureType castToTextureType(T value) { return static_cast<TextureType>(value); }
template <typename T> constexpr T castFromTextureType(TextureType value) { return static_cast<T>(value); }

enum class TextureInternalFormat : int32_t {
	Red8uiNorm, Red8iNorm, Red16f, Red32f, Red8uiUnnorm, Red8iUnnorm, Red16uiUnnorm, Red16iUnnorm, Red32uiUnnorm, Red32iUnnorm,
	RG8uiNorm, RG8iNorm, RG16f, RG32f, RG8uiUnnorm, RG8iUnnorm, RG16uiUnnorm, RG16iUnnorm, RG32uiUnnorm, RG32iUnnorm,
	RGB8uiNorm, RGB8iNorm, RGB16f, RGB32f, RGB8uiUnnorm, RGB8iUnnorm, RGB16uiUnnorm, RGB16iUnnorm, RGB32uiUnnorm, RGB32iUnnorm,
	RGBA8uiNorm, RGBA8iNorm, RGBA16f, RGBA32f, RGBA8uiUnnorm, RGBA8iUnnorm, RGBA16uiUnnorm, RGBA16iUnnorm, RGBA32uiUnnorm, RGBA32iUnnorm,
	Depth16ui, Depth24ui, Depth32f, Depth24uiStencil8ui, //Depth32fStencil8ui,
	// RGB565uiNorm, R11fG11fB10f, RGB9E5, RGB5uiA1uiNorm, RGBA4uiNorm, RGB10A2uiNorm, RGB10A2uiUnnorm
	Count,
};
template <typename T> constexpr TextureInternalFormat castToTextureInternalFormat(T value) { return static_cast<TextureInternalFormat>(value); }
template <typename T> constexpr T castFromTextureInternalFormat(TextureInternalFormat value) { return static_cast<T>(value); }

enum class TextureCubemapSide : int32_t {
	XNeg,
	XPos,
	YNeg,
	YPos,
	ZNeg,
	ZPos,
	Count
};
template <typename T> constexpr TextureCubemapSide castToTextureCubemapSide(T value) { return static_cast<TextureCubemapSide>(value); }
template <typename T> constexpr T castFromTextureCubemapSide(TextureCubemapSide value) { return static_cast<T>(value); }

enum class TextureMinificationFilter : int32_t {
	Nearest,
	Linear,
	NearestMipmapNearest,
	NearestMipmapLinear,
	LinearMipmapNearest,
	LinearMipmapLinear,
	Count
};
template <typename T> constexpr TextureMinificationFilter castToTextureMinificationFilter(T value) { return static_cast<TextureMinificationFilter>(value); }
template <typename T> constexpr T castFromTextureMinificationFilter(TextureMinificationFilter value) { return static_cast<T>(value); }

enum class TextureMagnificationFilter : int32_t {
	Nearest,
	Linear,
	Count
};
template <typename T> constexpr TextureMagnificationFilter castToTextureMagnificationFilter(T value) { return static_cast<TextureMagnificationFilter>(value); }
template <typename T> constexpr T castFromTextureMagnificationFilter(TextureMagnificationFilter value) { return static_cast<T>(value); }

enum class TextureWrapMode : int32_t {
	ClampToEdge,
	Repeat,
	MirroredRepeat,
	Count
};
template <typename T> constexpr TextureWrapMode castToTextureWrapMode(T value) { return static_cast<TextureWrapMode>(value); }
template <typename T> constexpr T castFromTextureWrapMode(TextureWrapMode value) { return static_cast<T>(value); }

enum class TextureSwizzle : int32_t {
	Red,
	Green,
	Blue,
	Alpha,
	Zero,
	One,
	Count
};
template <typename T> constexpr TextureSwizzle castToTextureSwizzle(T value) { return static_cast<TextureSwizzle>(value); }
template <typename T> constexpr T castFromTextureSwizzle(TextureSwizzle value) { return static_cast<T>(value); }

enum class TextureCompareFunc : int32_t {
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
template <typename T> constexpr TextureCompareFunc castToTextureCompareFunc(T value) { return static_cast<TextureCompareFunc>(value); }
template <typename T> constexpr T castFromTextureCompareFunc(TextureCompareFunc value) { return static_cast<T>(value); }

struct TextureSize {
	union { uint32_t width; uint32_t x; };
	union { uint32_t height; uint32_t y; };
	union { uint32_t depth; uint32_t z; };
	TextureSize(uint32_t w = 0, uint32_t h = 0, uint32_t d = 0) : width(w), height(h), depth(d) {}
};

class TexturePrivate;
class RENDERERSHARED_EXPORT Texture : public std::enable_shared_from_this<Texture> {
public:
	~Texture();

	ContextPtr context() const;
	TextureType type() const;
	TextureSize size() const;
	TextureInternalFormat internalFormat() const;
	uint32_t numLevels() const;

	void setSubImage(types::ImageFormat dataFormat, types::ImageType dataType, const void *pData, uint32_t level = 0, const TextureSize& offset = TextureSize(), renderer::TextureSize size = TextureSize());
	void setSubImage(TextureCubemapSide cubemapSide, types::ImageFormat dataFormat, types::ImageType dataType, const void *pData, uint32_t level = 0, const TextureSize& offset = TextureSize(), TextureSize size = TextureSize());
	void generateMipmap();

	void setFilter(TextureMinificationFilter minFilter, TextureMagnificationFilter magFilter);
	TextureMinificationFilter minificationFilter() const;
	TextureMagnificationFilter magnificationFilter() const;

	void setWrapMode(TextureWrapMode sMode = TextureWrapMode::Repeat, TextureWrapMode tMode = TextureWrapMode::Repeat, TextureWrapMode rMode = TextureWrapMode::Repeat);
	TextureWrapMode wrapModeS() const;
	TextureWrapMode wrapModeT() const;
	TextureWrapMode wrapModeR() const;

	void setSwizzle(TextureSwizzle rSwizzle = TextureSwizzle::Red, TextureSwizzle gSwizzle = TextureSwizzle::Green, TextureSwizzle bSwizzle = TextureSwizzle::Blue, TextureSwizzle aSwizzle = TextureSwizzle::Alpha);
	TextureSwizzle swizzleRed() const;
	TextureSwizzle swizzleGreen() const;
	TextureSwizzle swizzleBlue() const;
	TextureSwizzle swizzleAlpha() const;

	void enableCompareMode(TextureCompareFunc func = TextureCompareFunc::Less);
	void disableCompareMode();
	bool compareMode() const;
	TextureCompareFunc compareFunc() const;

private:
	Texture(ContextPtr context);
	Texture(ContextPtr context, TexturePtr sharedTexture);
	void init(TextureType type, int32_t numLevels, TextureInternalFormat internalFormat, const TextureSize& size);

	TexturePrivate *m;

	friend class Context;
	friend class ContextPrivate;
	friend class Framebuffer;
};

}

#endif // TEXTURE_H

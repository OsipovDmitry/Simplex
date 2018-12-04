#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>

#include "../utils/enumclass.h"
#include"../utils/pimpl.h"
#include"../utils/noncopyble.h"
#include"../utils/customdeleter.h"
#include "renderer_global.h"
#include "forwarddecl.h"

namespace types {
enum class ImageFormat : int32_t;
enum class ImageType : int32_t;
}

namespace renderer {

ENUMCLASS(TextureType, int32_t,
	Type_2D,
	Type_3D,
	Type_2DArray,
    Type_CubeMap
)


ENUMCLASS(TextureInternalFormat, int32_t,
    Red8uiNorm, Red8iNorm, Red16f, Red32f, Red8uiUnnorm, Red8iUnnorm, Red16uiUnnorm, Red16iUnnorm, Red32uiUnnorm, Red32iUnnorm,
	RG8uiNorm, RG8iNorm, RG16f, RG32f, RG8uiUnnorm, RG8iUnnorm, RG16uiUnnorm, RG16iUnnorm, RG32uiUnnorm, RG32iUnnorm,
	RGB8uiNorm, RGB8iNorm, RGB16f, RGB32f, RGB8uiUnnorm, RGB8iUnnorm, RGB16uiUnnorm, RGB16iUnnorm, RGB32uiUnnorm, RGB32iUnnorm,
	RGBA8uiNorm, RGBA8iNorm, RGBA16f, RGBA32f, RGBA8uiUnnorm, RGBA8iUnnorm, RGBA16uiUnnorm, RGBA16iUnnorm, RGBA32uiUnnorm, RGBA32iUnnorm,
    Depth16ui, Depth24ui, Depth32f, Depth24uiStencil8ui //,Depth32fStencil8ui,
	// RGB565uiNorm, R11fG11fB10f, RGB9E5, RGB5uiA1uiNorm, RGBA4uiNorm, RGB10A2uiNorm, RGB10A2uiUnnorm
)

ENUMCLASS(TextureCubemapSide, int32_t,
    XNeg,
	XPos,
	YNeg,
	YPos,
	ZNeg,
    ZPos
)

ENUMCLASS(TextureMinificationFilter, int32_t,
	Nearest,
	Linear,
	NearestMipmapNearest,
	NearestMipmapLinear,
	LinearMipmapNearest,
    LinearMipmapLinear
)

ENUMCLASS(TextureMagnificationFilter, int32_t,
	Nearest,
    Linear
)

ENUMCLASS(TextureWrapMode, int32_t,
	ClampToEdge,
	Repeat,
    MirroredRepeat
)

ENUMCLASS(TextureSwizzle, int32_t,
	Red,
	Green,
	Blue,
	Alpha,
	Zero,
    One
)

ENUMCLASS(TextureCompareFunc, int32_t,
	LessEqual,
	GreaterEqual,
	Less,
	Greater,
	Equal,
	NotEqual,
	Newer,
    Always_
)

struct TextureSize {
	union { uint32_t width; uint32_t x; };
	union { uint32_t height; uint32_t y; };
	union { uint32_t depth; uint32_t z; };
	TextureSize(uint32_t w = 0, uint32_t h = 0, uint32_t d = 0) : width(w), height(h), depth(d) {}
};

class TexturePrivate;
class RENDERERSHARED_EXPORT Texture {
    PIMPL(Texture)
    NONCOPYBLE(Texture)
    CUSTOMDELETER(Texture)

public:
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

    static TexturePtr create(ContextPtr context, TextureType type, TextureInternalFormat internalFormat, const TextureSize& size, int32_t numLevels = -1); // если numLevels == -1, то автоматически рассчитывается trunc(log2(size)) + 1

private:
    Texture(ContextPtr context, TextureType type, int32_t numLevels, TextureInternalFormat internalFormat, const TextureSize& size);
    ~Texture();

    TexturePrivate *m_;
};

}

#endif // TEXTURE_H

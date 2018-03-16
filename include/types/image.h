#ifndef IMAGE_H
#define IMAGE_H

#include <inttypes.h>

namespace types {

enum class ImageFormat : int32_t {
	Red,
	RG,
	RGB,
	RGBA,
	Depth,
	DepthStencil,
	Luminance,
	LuminanceAlpha,
	Alpha,
	Count
};
template <typename T> constexpr ImageFormat castToImageFormat(T value) { return static_cast<ImageFormat>(value); }
template <typename T> constexpr T castFromImageFormat(ImageFormat value) { return static_cast<T>(value); }

enum class ImageType : int32_t {
	Typy_8ui,
	Type_8i,
	Type_16ui,
	Type_16i,
	Type_32ui,
	Type_32i,
	Type_16f,
	Type_32f,
	Count
};
template <typename T> constexpr ImageType castToImageType(T value) { return static_cast<ImageType>(value); }
template <typename T> constexpr T castFromImageType(ImageType value) { return static_cast<T>(value); }

}

#endif // IMAGE_H

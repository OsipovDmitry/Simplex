#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>

#include "renderer_global.h"

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

class TexturePrivate;
class RENDERERSHARED_EXPORT Texture {
public:
	~Texture();

	ContextPtr context() const;

private:
	Texture(ContextPtr context, TextureType type);
	Texture(ContextPtr context, TexturePtr sharedTexture);

	TexturePrivate *m;

	friend class Context;
};

}

#endif // TEXTURE_H

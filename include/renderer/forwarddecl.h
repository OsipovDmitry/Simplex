#ifndef RENDERERFORWARDDECL_H
#define RENDERERFORWARDDECL_H

#include <memory>
#include <vector>

namespace renderer {

class DisplayPixelFormat;
using DisplayPixelFormatPtr = std::shared_ptr<DisplayPixelFormat>;
using DisplayPixelFormatsList = std::vector<DisplayPixelFormatPtr>;

class WindowSurface;
using WindowSurfacePtr = std::shared_ptr<WindowSurface>;

class Context;
using ContextPtr = std::shared_ptr<Context>;
using ContextWPtr = std::weak_ptr<Context>;
enum class DepthTestFunc : int32_t;
enum class BlendFunc : int32_t;
enum class BlendEquation : int32_t;

class ContextGroup;
using ContextGroupPtr = std::shared_ptr<ContextGroup>;

class Shader;
using ShaderPtr = std::shared_ptr<Shader>;
enum class ShaderType : int32_t;

class Program;
using ProgramPtr = std::shared_ptr<Program>;

class Buffer;
using BufferPtr = std::shared_ptr<Buffer>;
enum class BufferUsage : int32_t;

class VertexArray;
using VertexArrayPtr = std::shared_ptr<VertexArray>;
enum class VertexArrayAttributePointerType : int32_t;

class Texture;
using TexturePtr = std::shared_ptr<Texture>;
enum class TextureType : int32_t;
enum class TextureInternalFormat : int32_t;
enum class TextureCubemapSide : int32_t;
enum class TextureMinificationFilter : int32_t;
enum class TextureMagnificationFilter : int32_t;
enum class TextureWrapMode : int32_t;
enum class TextureSwizzle : int32_t;
enum class TextureCompareFunc : int32_t;
struct TextureSize;

class Renderbuffer;
using RenderbufferPtr = std::shared_ptr<Renderbuffer>;

class Framebuffer;
using FramebufferPtr = std::shared_ptr<Framebuffer>;
enum class FramebufferAttachment : int32_t;

}


#endif // RENDERERFORWARDDECL_H

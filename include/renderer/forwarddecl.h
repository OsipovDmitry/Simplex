#ifndef FORWARDDECL_H
#define FORWARDDECL_H

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

class Shader;
using ShaderPtr = std::shared_ptr<Shader>;
enum class ShaderType : int32_t;

class Program;
using ProgramPtr = std::shared_ptr<Program>;
using ProgramConstPtr = std::shared_ptr<const Program>;

class Buffer;
using BufferPtr = std::shared_ptr<Buffer>;
using BufferConstPtr = std::shared_ptr<const Buffer>;
enum class BufferUsage : int32_t;

class VertexArray;
using VertexArrayPtr = std::shared_ptr<VertexArray>;
using VertexArrayConstPtr = std::shared_ptr<const VertexArray>;

class Texture;
using TexturePtr = std::shared_ptr<Texture>;
using TextureConstPtr = std::shared_ptr<const Texture>;
enum class TextureType : int32_t;
enum class TextureInternalFormat : int32_t;
struct TextureSize;

class Renderbuffer;
using RenderbufferPtr = std::shared_ptr<Renderbuffer>;
using RenderbufferConstPtr = std::shared_ptr<const Renderbuffer>;

class Framebuffer;
using FramebufferPtr = std::shared_ptr<Framebuffer>;
using FramebufferConstPtr = std::shared_ptr<const Framebuffer>;

}


#endif // FORWARDDECL_H

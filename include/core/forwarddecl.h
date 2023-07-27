#ifndef CORE_FORWARDDECL_H
#define CORE_FORWARDDECL_H

#include <inttypes.h>

namespace simplex
{
namespace core
{

namespace graphics
{
enum class PixelInternalFormat : uint16_t;
enum class TextureType : uint16_t;
enum class TextureWrapMode : uint16_t;
enum class TextureFiltering : uint16_t;
enum class FrameBufferAttachment : uint16_t;
union FrameBufferClearColorValue;
enum class FrameBufferClearColorType : uint16_t;
enum class UniformId : uint16_t;
enum class UniformType : uint16_t;
enum class FaceType : uint16_t;
enum class DepthFunc : uint16_t;
enum class StencilFunc : uint16_t;
struct UniformInfo;
struct AttributeInfo;
class IBuffer;
class IVertexArray;
class ISurface;
class ITexture;
class IRenderBuffer;
class IFrameBuffer;
class IRenderProgram;
class IRenderer;
}

class INamedObject;
class IApplication;
class IEngine;

class RenderInfo;

class ApplicationBase;

class GraphicsEngine;

class RenderSurface;
class GSurface;

class AbstractUniform;
template <typename T> class Uniform;
class IDrawable;
class DrawableBase;
class StandardDrawable;

class Scene;
class Node;
class SceneRootNode;
class CameraNode;
class DrawableNode;

class NodeVisitor;
class ConditionalNodeVisitor;
class FrustumCullingVisitor;
template <typename NodeClass> class CollectorVisitor;

class RenderProgramsManager;

}
}

#endif // CORE_FORWARDDECL_H

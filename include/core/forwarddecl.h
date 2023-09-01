#ifndef CORE_FORWARDDECL_H
#define CORE_FORWARDDECL_H

#include <inttypes.h>
#include <memory>

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
enum class SSBOId : uint16_t;
struct AttributeInfo;
struct UniformInfo;
struct SSBOInfo;
enum class FaceType : uint16_t;
enum class DepthFunc : uint16_t;
enum class StencilFunc : uint16_t;
class IBuffer;
class IBufferRange;
using PBufferRange = std::shared_ptr<const IBufferRange>;
class IVertexArray;
class ISurface;
class ITexture;
using PTexture = std::shared_ptr<const ITexture>;
class IImage;
using PImage = std::shared_ptr<const IImage>;
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

class AbstractUniform;
using PAbstratcUniform = std::shared_ptr<AbstractUniform>;

template <typename T> class Uniform;
template<typename T>
using PUniform = std::shared_ptr<Uniform<T>>;

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
class FrustumCullingNodeVisitor;
template <typename NodeClass> class CollectorVisitor;

class RenderProgramsManager;

}
}

#endif // CORE_FORWARDDECL_H

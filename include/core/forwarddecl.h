#ifndef CORE_FORWARDDECL_H
#define CORE_FORWARDDECL_H

#include <inttypes.h>
#include <memory>
#include <unordered_set>

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
enum class PBRComponent : uint16_t;
using PBRComponentsSet = std::unordered_set<PBRComponent>;
enum class FaceType : uint16_t;
enum class DepthFunc : uint16_t;
enum class StencilFunc : uint16_t;
class IBuffer;
class IBufferRange;
using PConstBufferRange = std::shared_ptr<const IBufferRange>;
class IVertexArray;
class ISurface;
class ITexture;
using PConstTexture = std::shared_ptr<const ITexture>;
class IImage;
using PConstImage = std::shared_ptr<const IImage>;
class IRenderBuffer;
class IFrameBuffer;
class IProgram;
class IRenderProgram;
class IComputeProgram;
class IRenderer;
}

namespace debug
{
class GraphicsEngineInformation;
}

class Settings;

class INamedObject;
class IApplication;
class IEngine;

class RenderInfo;

class ApplicationBase;

class GraphicsEngine;

class AbstractUniform;
using PAbstractUniform = std::shared_ptr<AbstractUniform>;
using PConstAbstractUniform = std::shared_ptr<const AbstractUniform>;

template <typename T> class Uniform;
template<typename T>
using PUniform = std::shared_ptr<Uniform<T>>;

class Drawable;
class StandardDrawable;

class Scene;
class Node;
class SceneRootNode;
class CameraNode;
class DrawableNode;
class LightNode;
class PointLightNode;
class SpotLightNode;
class DirectionalLightNode;

class NodeVisitor;
template <typename NodeClass> class NodeCollector;

class TexturesManager;
class ProgramsManager;

}
}

#endif // CORE_FORWARDDECL_H

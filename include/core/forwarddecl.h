#ifndef CORE_FORWARDDECL_H
#define CORE_FORWARDDECL_H

#include <inttypes.h>
#include <memory>
#include <array>
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
enum class TextureFilterMode : uint16_t;
enum class TextureSwizzle : uint16_t;
using TextureSwizzleMask = std::array<TextureSwizzle, 4u>;
enum class FrameBufferAttachment : uint16_t;
union FrameBufferClearColorValue;
enum class FrameBufferClearColorType : uint16_t;
enum class UniformId : uint16_t;
enum class UniformType : uint16_t;
enum class SSBOId : uint16_t;
struct AttributeInfo;
struct UniformInfo;
struct SSBOVariableInfo;
struct SSBOInfo;
enum class FaceType : uint16_t;
enum class ComparingFunc : uint16_t;
enum class StencilOperation : uint16_t;
enum class BlendEquation : uint16_t;
enum class BlendFactor : uint16_t;
using StencilOperations = std::array<StencilOperation, 3u>; // sfail, dpfail, dppass
class IBuffer;
class IBufferRange;
class IVertexArray;
class ISurface;
class ITexture;
class IImage;
class IRenderBuffer;
class IFrameBuffer;
class IProgram;
class IRenderProgram;
class IComputeProgram;
class IRenderer;

using PConstBufferRange = std::shared_ptr<const IBufferRange>;
using PBufferRange = std::shared_ptr<IBufferRange>;

using PConstTexture = std::shared_ptr<const ITexture>;
using PTexture = std::shared_ptr<ITexture>;

using PConstImage = std::shared_ptr<const IImage>;
using PImage = std::shared_ptr<IImage>;
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

enum class DrawableAlphaMode : uint16_t;
class Drawable;

enum class PBRComponent : uint16_t;
class PBRDrawable;

enum class LightComponent : uint16_t;
class LightDrawable;

enum class BackgroundComponent : uint16_t;
class BackgroundDrawable;

class Scene;
class Node;
class SceneRootNode;
class CameraNode;
class DrawableNode;
enum class LightType : uint16_t;
enum class LightShadingMode : uint16_t;
class LightNode;
class PointLightNode;
class SpotLightNode;
class DirectionalLightNode;
class IBLLightNode;

class NodeVisitor;
template <typename NodeClass> class NodeCollector;

class TexturesManager;
class ProgramsManager;

}
}

#endif // CORE_FORWARDDECL_H

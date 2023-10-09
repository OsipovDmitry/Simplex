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
enum class LightComponent : uint16_t;
using LightComponentsSet = std::unordered_set<LightComponent>;
enum class BackgroundComponent : uint16_t;
using BackgroundComponentsSet = std::unordered_set<BackgroundComponent>;
enum class FaceType : uint16_t;
enum class ComparingFunc : uint16_t;
enum class StencilOperation : uint16_t;
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

class Drawable;
class StandardDrawable;
enum class LightDrawableType : uint16_t;
class LightDrawable;

class Scene;
class Node;
class SceneRootNode;
class CameraNode;
class DrawableNode;
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

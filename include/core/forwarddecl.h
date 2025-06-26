#ifndef CORE_FORWARDDECL_H
#define CORE_FORWARDDECL_H

#include <inttypes.h>
#include <memory>
#include <array>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <string>

namespace simplex
{
namespace core
{

namespace debug
{
struct CameraInformation;
struct SceneInformation;
struct Information;
}

namespace settings
{
class Application;
class Camera;
class Background;
class Flat;
class PBR;
class IBL;
class Shadow;
class SSAO;
class Blur;
class NodeBoundingBox;
class VisualDrawableNodeLocalBoundingBox;
class VisualDrawableBoundingBox;
class LightNodeAreaBoundingBox;
class DebugRendering;
class Graphics;
class Audio;
class Physics;
class Settings;
}

namespace graphics
{
enum class PixelInternalFormat : uint16_t;
using SupportedImageFormats = std::unordered_map<PixelInternalFormat, std::string>;
enum class TextureType : uint16_t;
enum class TextureWrapMode : uint16_t;
enum class TextureFilterMode : uint16_t;
enum class TextureSwizzle : uint16_t;
using TextureSwizzleMask = std::array<TextureSwizzle, 4u>;
enum class FrameBufferAttachment : uint16_t;
union FrameBufferClearColorValue;
enum class FrameBufferClearColorType : uint16_t;
enum class UniformType : uint16_t;
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

namespace audio
{
enum class BufferFormat : uint16_t;
enum class SourceState : uint16_t;
enum class AttenuationModel : uint16_t;
class IBuffer;
class ISource;
class IListener;
class IRenderer;
}

class INamedObject;
class IEngine;

class IRenderWidget;
class ApplicationBase;

class RenderInfo;

class GraphicsEngine;
class AudioEngine;

class AbstractUniform;
using PAbstractUniform = std::shared_ptr<AbstractUniform>;
using PConstAbstractUniform = std::shared_ptr<const AbstractUniform>;

template <typename T> class Uniform;
template<typename T>
using PUniform = std::shared_ptr<Uniform<T>>;

enum class UniformId : uint16_t;
using UniformCollection = std::unordered_map<UniformId, PAbstractUniform>;
using UserUniformCollection = std::unordered_map<std::string, PAbstractUniform>;
enum class SSBOId : uint16_t;
using SSBOCollection = std::unordered_map<SSBOId, graphics::PConstBufferRange>;
class StateSet;
using PStateSet = std::shared_ptr<StateSet>;
using PConstStateSet = std::shared_ptr<const StateSet>;

class Drawable;
using DrawableComponentSet = std::set<UniformId>;
class VisualDrawable;
class FlatDrawable;
class PBRDrawable;

class Animation;
class Background;
class Shadow;
class Skeleton;
class SSAO;

class Scene;
class Node;
class SceneRootNode;
enum class SSAOMode : uint16_t;
class CameraNode;
class VisualDrawableNode;
enum class LightType : uint16_t;
enum class ShadingMode : uint16_t;
enum class ShadingFilter : uint16_t;
class LightNode;
class PointLightNode;
class SpotLightNode;
class DirectionalLightNode;
class IBLLightNode;
enum class SoundState : uint16_t;
class SoundNode;
class ListenerNode;

class NodeVisitor;
template <typename NodeClass> class NodeCollector;

class TexturesManager;
class ProgramsManager;
class SoundsManager;

}
}

#endif // CORE_FORWARDDECL_H

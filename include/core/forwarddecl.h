#ifndef CORE_FORWARDDECL_H
#define CORE_FORWARDDECL_H

#include <inttypes.h>
#include <array>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <string>
#include <functional>
#include <memory>

#include <utils/forwarddecl.h>
#include <utils/glm/vec2.hpp>

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
class DrawableNodeLocalBoundingBox;
class DrawableBoundingBox;
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
struct OutputInfo;
struct UniformInfo;
struct BufferVariableInfo;
struct UniformBlockInfo;
struct ShaderStorageBlockInfo;
enum class FaceType : uint16_t;
enum class ComparingFunc : uint16_t;
enum class StencilOperation : uint16_t;
enum class BlendEquation : uint16_t;
enum class BlendFactor : uint16_t;
using StencilOperations = std::array<StencilOperation, 3u>; // sfail, dpfail, dppass
class IBuffer;
class IStaticBuffer;
class IDynamicBuffer;
class IVertexArray;
class ISurface;
class ITexture;
using TextureHandle = uint64_t;
using ImageHandle = uint64_t;
class ITextureHandle;
class IImageHandle;
class IRenderBuffer;
class IFrameBuffer;
class IProgram;
class IRenderProgram;
class IComputeProgram;
struct DrawArraysIndirectCommand;
struct DrawElementsIndirectCommand;
struct DrawIndirectCommandsBufferReservedData;
class RendererBase;
class BufferRange;
class Image;
class VAOMesh;
template <typename T> class StructBuffer;
template <typename T> class VectorBuffer;

using PConstBuffer = std::shared_ptr<const IBuffer>;
using PBuffer = std::shared_ptr<IBuffer>;

using PConstBufferRange = std::shared_ptr<const BufferRange>;
using PBufferRange = std::shared_ptr<BufferRange>;

using PConstTexture = std::shared_ptr<const ITexture>;
using PTexture = std::shared_ptr<ITexture>;

using PConstTextureHandle = std::shared_ptr<const ITextureHandle>;
using PTextureHandle = std::shared_ptr<ITextureHandle>;

using PConstImage = std::shared_ptr<const Image>;
using PImage = std::shared_ptr<Image>;

using PConstImageHandle = std::shared_ptr<const IImageHandle>;
using PImageHandle = std::shared_ptr<IImageHandle>;

using DrawArraysIndirectCommandsBuffer = VectorBuffer<DrawArraysIndirectCommand>;
using PDrawArraysIndirectCommandsBuffer = std::shared_ptr<DrawArraysIndirectCommandsBuffer>;
using PDrawArraysIndirectCommandsConstBuffer = std::shared_ptr<const DrawArraysIndirectCommandsBuffer>;

using DrawElementsIndirectCommandBuffer = VectorBuffer<DrawElementsIndirectCommand>;
using PDrawElementsIndirectCommandBuffer = std::shared_ptr<DrawElementsIndirectCommandBuffer>;
using PDrawElementsIndirectCommandConstBuffer = std::shared_ptr<const DrawElementsIndirectCommandBuffer>;

class IGraphicsWidget;
enum class KeyState : uint16_t;
enum class KeyCode : uint16_t;
enum class KeyModifier : uint16_t;
using KeyModifiers = std::unordered_set<KeyModifier>;
enum class MouseButtonState : uint16_t;
enum class MouseButton : uint16_t;
enum class MouseStandardCursor : uint16_t;
using CloseCallback = std::function<void(void)>;
using ResizeCallback = std::function<void(const glm::uvec2&)>;
using UpdateCallback = std::function<void(uint64_t, uint32_t)>;
using KeyCallback = std::function<void(KeyState, KeyCode, const KeyModifiers&)>;
using MouseCursorMoveCallback = std::function<void(const glm::ivec2&)>;
using MouseCursorEnterCallback = std::function<void(bool)>;
using MouseButtonCallback = std::function<void(MouseButtonState, MouseButton, const KeyModifiers&)>;
using MouseScrollCallback = std::function<void(const glm::ivec2&)>;

}

namespace audio
{
enum class BufferFormat : uint16_t;
enum class SourceState : uint16_t;
enum class AttenuationModel : uint16_t;
class IBuffer;
class ISource;
class IListener;
class RendererBase;
class IAudioDevice;
}

class INamedObject;

class IRenderer;

class IDevice;
class ApplicationBase;
using ApplicationTimeCallback = std::function<uint64_t()>;
using ApplicationPollEventsCallback = std::function<void()>;

class IEngine;
class GraphicsEngine;
class AudioEngine;

class AbstractUniform;
using PAbstractUniform = std::shared_ptr<AbstractUniform>;
using PConstAbstractUniform = std::shared_ptr<const AbstractUniform>;

template <typename T> class Uniform;
template<typename T>
using PUniform = std::shared_ptr<Uniform<T>>;

enum class UniformID : uint16_t;
using UniformCollection = std::unordered_map<UniformID, PAbstractUniform>;
using UserUniformCollection = std::unordered_map<std::string, PAbstractUniform>;
enum class UniformBlockID : uint16_t;
using UniformBlockCollection = std::unordered_map<UniformBlockID, graphics::PConstBufferRange>;
enum class ShaderStorageBlockID : uint16_t;
using ShaderStorageBlockCollection = std::unordered_map<ShaderStorageBlockID, graphics::PConstBufferRange>;
class StateSet;
using StateSetList = std::list<std::shared_ptr<const StateSet>>;

class Mesh;

class MaterialMap;
enum class MaterialMapTarget: uint16_t;
class Material;

class Drawable;
using DrawableComponentSet = std::set<UniformID>;

class Background;
class Shadow;
class SSAO;
enum class SSAOMode : uint16_t;
enum class LightType : uint16_t;
enum class ShadingMode : uint16_t;
enum class ShadingFilter : uint16_t;
enum class SoundState : uint16_t;

struct AnimationChannel;
class Animation;
struct Bone;
class Skeleton;

class Scene;
class Node;
class SceneRootNode;
class DrawableNode;
class CameraNode;
class LightNode;
class PointLightNode;
class SpotLightNode;
class DirectionalLightNode;
class ImageBasedLightNode;
class BoneNode;
class SkeletalAnimatedNode;
class SoundNode;
class ListenerNode;

class NodeVisitor;
template <typename NodeClass> class NodeCollector;

class NodeRepresentation;
class DrawableNodeRepresentation;
class BoneNodeRepresentation;
class RootBoneNodeRepresentation;
class CameraNodeRepresentation;
class LightNodeRepresentation;
class SceneRepresentation;

class TexturesManager;
class ProgramsManager;
class ScenesRepresentationsManager;
class SoundsManager;

}
}

#endif // CORE_FORWARDDECL_H

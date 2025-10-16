#ifndef CORE_IGRAPHICSRENDERER_H
#define CORE_IGRAPHICSRENDERER_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <utils/glm/vec2.hpp>
#include <utils/glm/vec3.hpp>
#include <utils/glm/vec4.hpp>
#include <utils/glm/mat2x2.hpp>
#include <utils/glm/mat3x3.hpp>
#include <utils/glm/mat4x4.hpp>
#include <utils/image.h>
#include <utils/mesh.h>
#include <utils/logger.h>

#include <utils/forwarddecl.h>
#include <utils/enumclass.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>
#include <core/irenderer.h>

namespace simplex
{
namespace core
{
namespace graphics
{

ENUMCLASS(PixelInternalFormat, uint16_t,
          R8, R8_SNORM, R16, R16_SNORM,
          RG8, RG8_SNORM, RG16, RG16_SNORM,
          R3_G3_B2, RGB4, RGB5, RGB8, RGB8_SNORM, RGB10, RGB12, RGB16, RGB16_SNORM, RGBA2,
          RGBA4, RGB5_A1, RGBA8, RGBA8_SNORM, RGB10_A2, RGB10_A2UI, RGBA12, RGBA16, RGBA16_SNORM,
          SRGB8, SRGB8_ALPHA8,
          R16F, RG16F, RGB16F, RGBA16F,
          R32F, RG32F, RGB32F, RGBA32F,
          R11F_G11F_B10F, RGB9_E5,
          R8I, R8UI, R16I, R16UI, R32I, R32UI,
          RG8I, RG8UI, RG16I, RG16UI, RG32I, RG32UI,
          RGB8I, RGB8UI, RGB16I, RGB16UI, RGB32I, RGB32UI,
          RGBA8I, RGBA8UI, RGBA16I, RGBA16UI, RGBA32I, RGBA32UI,
          Depth16, Depth24, Depth32F, Stencil8, Depth24Stencil8, Dept32FStencil8)

CORE_SHARED_EXPORT core::graphics::PixelInternalFormat pixelNumComponentsAndPixelComponentTypeToPixelInternalFormat(
    uint32_t,
    utils::PixelComponentType);

ENUMCLASS(TextureType, uint16_t,
          Type1D,
          Type2D,
          Type3D,
          TypeCube,
          Type1DArray,
          Type2DArray,
          TypeCubeArray,
          TypeRect)

ENUMCLASS(TextureWrapMode, uint16_t,
          ClampToEdge,
          ClampToBorder,
          MirroredRepeat,
          Repeat,
          MirrorClampToEdge)

ENUMCLASS(TextureFilterMode, uint16_t,
          Point,
          Linear,
          Bilinear,
          Trilinear)

ENUMCLASS(TextureSwizzle, uint16_t,
          Red,
          Green,
          Blue,
          Alpha,
          Zero,
          One)

ENUMCLASS(FrameBufferAttachment, uint16_t,
          Color0,
          Color1,
          Color2,
          Color3,
          Depth,
          Stencil,
          DepthStencil)

union FrameBufferClearColorValue
{
    glm::vec4 floatColor;
    glm::i32vec4 intColor;
    glm::u32vec4 uintColor;
};

ENUMCLASS(FrameBufferClearColorType, uint16_t,
          Single,
          Int32,
          Uint32)

using FrameBufferClearColor = std::pair<FrameBufferClearColorType, FrameBufferClearColorValue>;

ENUMCLASS(UniformType, uint16_t,
          Bool,
          Single,
          SingleVec2,
          SingleVec3,
          SingleVec4,
          SingleMat2,
          SingleMat3,
          SingleMat4,
          Double,
          DoubleVec2,
          DoubleVec3,
          DoubleVec4,
          DoubleMat2,
          DoubleMat3,
          DoubleMat4,
          Int32,
          Int32Vec2,
          Int32Vec3,
          Int32Vec4,
          Uint32,
          Uint32Vec2,
          Uint32Vec3,
          Uint32Vec4,
          Sampler1D,
          Sampler2D,
          Sampler3D,
          SamplerCube,
          Sampler1DArray,
          Sampler2DArray,
          SamplerCubeArray,
          SamplerRect,
          Image1D,
          Image2D,
          Image3D,
          ImageCube,
          Image1DArray,
          Image2DArray,
          ImageCubeArray,
          ImageRect,
          AtomicCounterUint)

struct AttributeInfo
{
    utils::VertexAttribute ID;
    uint16_t index;
    int32_t location;
    uint16_t numComponents;
    utils::VertexComponentType componentType;
};

struct OutputInfo
{
    FrameBufferAttachment ID;
    uint16_t index;
    int32_t location;
};

struct UniformInfo
{
    UniformID ID;
    uint16_t index;
    int32_t location;
    UniformType type;
};

struct BufferVariableInfo
{
    uint16_t index;
    //VariableType type;
    uint16_t arraySize;
    uint16_t offset;
    uint16_t arrayStride;
    uint16_t matrixStride;
    uint16_t topLevelArraySize;
    uint16_t topLevelArrayStride;
};

struct UniformBlockInfo
{
    UniformBlockID ID;
    uint16_t index;
    std::vector<BufferVariableInfo> variables;
};

struct ShaderStorageBlockInfo
{
    ShaderStorageBlockID ID;
    uint16_t index;
    std::vector<BufferVariableInfo> variables;
};

ENUMCLASS(FaceType, uint16_t,
          Front, Back, FrontAndBack)

ENUMCLASS(ComparingFunc, uint16_t,
          Never, Less, Equal, LessOrEqual, Greater, NotEqual, GreatOrEqual, Always)

ENUMCLASS(StencilOperation, uint16_t,
          Keep, Zero, Replace, Increment, IncrementWrap, Decrement, DecrementWrap, Invert)

ENUMCLASS(BlendEquation, uint16_t,
          Add, Sub, InvSub, Min, Max)

ENUMCLASS(BlendFactor, uint16_t,
          Zero, One,
          SrcColor, OneMinusSrcColor, DstColor, OneMinusDstColor,
          SrcAlpha, OneMinusSrcAlpha, DstAlpha, OneMinusDstAlpha,
          ConstColot, OneMinusConstColor, ConstAlpha, OneMinusConstAlpha)

static constexpr FrameBufferAttachment FrameBufferColorAttachment(uint16_t i) {
    return castToFrameBufferAttachment(castFromFrameBufferAttachment(FrameBufferAttachment::Color0) + i);
}

static constexpr uint16_t FrameBufferColorAttachmentIndex(FrameBufferAttachment a) {
    return castFromFrameBufferAttachment(a) - castFromFrameBufferAttachment(FrameBufferAttachment::Color0);
}

static constexpr uint16_t FrameBufferColorAttachmentsCount() {
    return FrameBufferColorAttachmentIndex(FrameBufferAttachment::Depth) - FrameBufferColorAttachmentIndex(FrameBufferAttachment::Color0);
}

static constexpr bool IsFrameBufferColorAttachment(FrameBufferAttachment a) {
    return FrameBufferColorAttachmentIndex(a) < FrameBufferColorAttachmentsCount();
}

class IBuffer
{
public:
    virtual ~IBuffer() = default;

    virtual bool isEmpty() const = 0;
    virtual size_t size() const = 0;

    class MappedData {
    public:
        virtual ~MappedData() = default; // mapped data has to be unmapped in destructor's implementation
        virtual uint8_t* get() = 0;
        virtual const uint8_t* get() const = 0;
    };

    ENUMCLASS(MapAccess, uint8_t, ReadOnly, WriteOnly, ReadWrite)
    virtual std::unique_ptr<MappedData> map(MapAccess access, size_t offset = 0u, size_t size = 0u) = 0;
    virtual std::unique_ptr<const MappedData> map(MapAccess access, size_t offset = 0u, size_t size = 0u) const = 0;
};

class IStaticBuffer : public IBuffer
{
public:
};

class IDynamicBuffer : public IBuffer
{
public:
    virtual size_t capacity() const = 0;
    virtual void reserve(size_t) = 0;
    virtual void shrinkToFit() = 0;

    virtual void clear() = 0;
    virtual void insert(size_t offset, const void* data, size_t size) = 0;
    virtual void erase(size_t offset, size_t size) = 0;
    virtual void resize(size_t) = 0;
};

class IVertexArray
{
public:
    virtual ~IVertexArray() = default;

    virtual uint32_t attachVertexBuffer(std::shared_ptr<IBuffer> buffer, size_t offset, size_t stride) = 0; // returns bindingIndex
    virtual void detachVertexBuffer(uint32_t bindingIndex) = 0;
    virtual std::shared_ptr<const IBuffer> vertexBuffer(uint32_t bindingIndex) const = 0;
    virtual size_t vertexBufferOffset(uint32_t bindingIndex) const = 0;
    virtual size_t vertexBufferStride(uint32_t bindingIndex) const = 0;

    virtual void declareVertexAttribute(utils::VertexAttribute,
                                        uint32_t bindingIndex,
                                        uint32_t numComponents,
                                        utils::VertexComponentType type,
                                        uint32_t relativeOffset) = 0;
    virtual void undeclareVertexAttribute(utils::VertexAttribute) = 0;
    virtual uint32_t vertexAttributeBindingIndex(utils::VertexAttribute) const = 0;
    virtual uint32_t vertexAttributeNumComponents(utils::VertexAttribute) const = 0;
    virtual utils::VertexComponentType vertexAttributeComponentType(utils::VertexAttribute) const = 0;
    virtual uint32_t vertexAttributeRelativeOffset(utils::VertexAttribute) const = 0;

    virtual void attachIndexBuffer(std::shared_ptr<IBuffer> buffer) = 0;
    virtual void detachIndexBuffer() = 0;
    virtual std::shared_ptr<const IBuffer> indexBuffer() const = 0;
};

class ISurface
{
public:
    virtual ~ISurface() = default;

    virtual glm::uvec2 size() const = 0;
    virtual PixelInternalFormat internalFormat() const = 0;
    virtual bool hasAlpha() const = 0;
    virtual bool hasDepth() const = 0;
};

class ITexture : public ISurface
{
public:
    virtual TextureType type() const = 0;

    virtual glm::uvec3 mipmapSize(uint32_t level = 0) const = 0;
    virtual uint32_t numMipmapLevels() const = 0;
    virtual uint32_t numFaces() const = 0;

    virtual void setSubImage(uint32_t level,
                             const glm::uvec3 &offset,
                             const glm::uvec3 &size,
                             uint32_t numComponents,
                             utils::PixelComponentType type,
                             const void *data) = 0;

    virtual void subImage(uint32_t level,
                          const glm::uvec3 &offset,
                          const glm::uvec3 &size,
                          uint32_t numComponents,
                          utils::PixelComponentType type,
                          size_t bufSize,
                          void *data) const = 0;

    virtual void generateMipmaps() = 0;
    virtual void setBorderColor(const glm::vec4&) = 0;
    virtual void setWrapMode(TextureWrapMode) = 0;
    virtual void setFilterMode(TextureFilterMode) = 0;
    virtual void setSwizzleMask(const TextureSwizzleMask&) = 0;

    virtual PTexture copyEmpty() const = 0;
    virtual PTexture copy() const = 0;

};

class ITextureHandle
{
public:
    virtual ~ITextureHandle() = default;

    virtual TextureHandle handle() const = 0;

    virtual PConstTexture texture() const = 0;

    virtual void makeResident() = 0;
    virtual void doneResident() = 0;
};

class IRenderBuffer : public ISurface
{
public:
};

class IFrameBuffer
{
public:
    virtual ~IFrameBuffer() = default;

    virtual bool isComplete() const = 0;
    
    virtual void attach(FrameBufferAttachment, std::shared_ptr<const ISurface>, uint32_t level = 0u) = 0;
    virtual void attachLayer(FrameBufferAttachment, std::shared_ptr<const ITexture>, uint32_t level = 0u, uint32_t layer = 0u) = 0;
    virtual void detach(FrameBufferAttachment) = 0;
    virtual void detachAll() = 0;
    virtual std::shared_ptr<const ISurface> attachmentSurface(FrameBufferAttachment) const = 0;
    virtual uint32_t attachmentMipmapLevel(FrameBufferAttachment) const = 0;
    virtual uint32_t attachmentLayer(FrameBufferAttachment) const = 0;

    virtual const FrameBufferClearColor &clearColor(uint32_t) const = 0;
    virtual void setClearColor(uint32_t, const glm::vec4&) = 0;
    virtual void setClearColor(uint32_t, const glm::i32vec4&) = 0;
    virtual void setClearColor(uint32_t, const glm::u32vec4&) = 0;

    virtual float clearDepth() const = 0;
    virtual int32_t clearStencil() const = 0;
    virtual void setClearDepthStencil(float, uint8_t) = 0;

    virtual const std::unordered_set<FrameBufferAttachment> &clearMask() const = 0;
    virtual void setClearMask(const std::unordered_set<FrameBufferAttachment>&) = 0;

    virtual bool faceCulling() const = 0;
    virtual FaceType cullFaceType() const = 0;
    virtual void setFaceCulling(bool, FaceType = FaceType::Back) = 0;

    virtual bool colorMask(uint32_t) const = 0;
    virtual void setColorMask(uint32_t, bool) = 0;
    virtual void setColorMasks(bool) = 0;

    virtual bool depthTest() const = 0;
    virtual ComparingFunc depthFunc() const = 0;
    virtual void setDepthTest(bool, ComparingFunc func = ComparingFunc::Less) = 0;
    virtual bool depthMask() const = 0;
    virtual void setDepthMask(bool) = 0;

    virtual bool stencilTest() const = 0;
    virtual void setStencilTest(bool) = 0;
    virtual ComparingFunc stencilComparingFunc(FaceType) const = 0;
    virtual uint8_t stencilReferenceValue(FaceType) const = 0;
    virtual uint8_t stencilMaskValue(FaceType) const = 0;
    virtual void setStencilFunc(FaceType, ComparingFunc, uint8_t ref, uint8_t mask) = 0;
    virtual const StencilOperations &stencilOperations(FaceType) const = 0;
    virtual void setStencilOperations(FaceType, const StencilOperations&) = 0;

    virtual bool blending() const = 0;
    virtual void setBlending(bool) = 0;
    virtual BlendEquation blendColorEquation(uint32_t) const = 0;
    virtual BlendEquation blendAlphaEquation(uint32_t) const = 0;
    virtual void setBlendEquation(uint32_t, BlendEquation, BlendEquation) = 0;
    virtual BlendFactor blendColorSourceFactor(uint32_t) const = 0;
    virtual BlendFactor blendAlphaSourceFactor(uint32_t) const = 0;
    virtual BlendFactor blendColorDestinationFactor(uint32_t) const = 0;
    virtual BlendFactor blendAlphaDestinationFactor(uint32_t) const = 0;
    virtual void setBlendFactor(uint32_t, BlendFactor srcColor, BlendFactor dstColor, BlendFactor srcAlpha, BlendFactor dstAlpha) = 0;
    virtual glm::vec3 blendConstantColor() const = 0;
    virtual void setBlendConstantColor(const glm::vec3&) = 0;
    virtual float blendConstantAlpha() const = 0;
    virtual void setBlendConstantAlpha(float) = 0;
};

class IProgram
{
public:
    virtual ~IProgram() = default;

    //virtual int32_t uniformLocationByName(const std::string&) const = 0;

    virtual const std::vector<UniformInfo> &uniformsInfo() const = 0;
    virtual const std::vector<UniformBlockInfo>& uniformBlocksInfo() const = 0;
    virtual const std::vector<ShaderStorageBlockInfo> &shaderStorageBlocksInfo() const = 0;

    virtual std::string uniformNameByIndex(uint16_t) const = 0;
    virtual std::string bufferVariableNameByIndex(uint16_t) const = 0;
    virtual std::string uniformBlockNameByIndex(uint16_t) const = 0;
    virtual std::string shaderStorageBlockNameByIndex(uint16_t) const = 0;

    static UniformType uniformTypeByTextureType(TextureType);
    static UniformType uniformTypeByImageTextureType(TextureType);
};

class IRenderProgram : public virtual IProgram
{
public:
    //virtual int32_t attributeLocationByName(const std::string&) const = 0;
    virtual const std::vector<AttributeInfo> &attributesInfo() const = 0;
    virtual std::string attributeNameByIndex(uint16_t) const = 0;

    virtual const std::vector<OutputInfo>& outputsInfo() const = 0;
    virtual std::string outputNameByIndex(uint16_t) const = 0;
};

class IComputeProgram : public virtual IProgram
{
public:
    virtual glm::uvec3 workGroupSize() const = 0;
};

struct DrawArraysIndirectCommand
{
    uint32_t count;
    uint32_t instanceCount;
    uint32_t first;
    uint32_t baseInstance;
};

struct DrawElementsIndirectCommand
{
    uint32_t count;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t  baseVertex;
    uint32_t baseInstance;
};

struct DrawIndirectCommandsBufferReservedData
{
    uint32_t count;
    uint32_t maxSize;
    uint32_t padding[2u];
};

class RendererBasePrivate;
class CORE_SHARED_EXPORT RendererBase : public std::enable_shared_from_this<RendererBase>, public IRenderer
{
public:
    RendererBase(const std::string&);
    ~RendererBase() override;

    const std::string& name() const override;

    bool makeCurrent() override final;
    bool doneCurrent() override final;
    static std::shared_ptr<RendererBase> current();
    static bool areShared(const std::shared_ptr<const RendererBase>&, const std::shared_ptr<const RendererBase>&);

    bool registerAttribute(const std::string&, utils::VertexAttribute);
    bool unregisterAttribute(const std::string&);
    utils::VertexAttribute attributeByName(const std::string&) const;

    bool registerOutput(const std::string&, FrameBufferAttachment);
    bool unregisterOutput(const std::string&);
    FrameBufferAttachment outputByName(const std::string&) const;

    bool registerUniform(const std::string&, UniformID);
    bool unregisterUniform(const std::string&);
    UniformID uniformByName(const std::string&) const;

    bool registerUniformBlock(const std::string&, UniformBlockID);
    bool unregisterUniformBlock(const std::string&);
    UniformBlockID uniformBlockByName(const std::string&) const;

    bool registerShaderStorageBlock(const std::string&, ShaderStorageBlockID);
    bool unregisterShaderStorageBlock(const std::string&);
    ShaderStorageBlockID shaderStorageBlockByName(const std::string&) const;

    virtual std::shared_ptr<IGraphicsWidget> widget() = 0;
    virtual std::shared_ptr<const IGraphicsWidget> widget() const = 0;

    virtual void blitFrameBuffer(std::shared_ptr<const IFrameBuffer> src,
                                 std::shared_ptr<IFrameBuffer> dst,
                                 const glm::uvec4 &srcViewport,
                                 const glm::uvec4 &dstViewport,
                                 bool colorMsk, bool depthMask, bool stencilMask,
                                 bool linearFilter = false) = 0;

    virtual std::shared_ptr<IStaticBuffer> createStaticBuffer(size_t size = 0u, const void *data = nullptr) const = 0;
    virtual std::shared_ptr<IDynamicBuffer> createDynamicBuffer(size_t size = 0u, const void* data = nullptr) const = 0;
    virtual std::shared_ptr<IVertexArray> createVertexArray() const = 0;
    virtual std::shared_ptr<ITexture> createTexture1DEmpty(uint32_t width,
                                                           PixelInternalFormat,
                                                           uint32_t numLevels = 1) const = 0;
    virtual std::shared_ptr<ITexture> createTexture1D(const std::shared_ptr<const utils::Image>&,
                                                      PixelInternalFormat = PixelInternalFormat::Count,
                                                      uint32_t numLevels = 0,
                                                      bool genMipmaps = true) const = 0;
    virtual std::shared_ptr<ITexture> createTexture2DEmpty(uint32_t width,
                                                           uint32_t height,
                                                           PixelInternalFormat,
                                                           uint32_t numLevels = 1) const = 0;
    virtual std::shared_ptr<ITexture> createTexture2D(const std::shared_ptr<const utils::Image>&,
                                                      PixelInternalFormat = PixelInternalFormat::Count,
                                                      uint32_t numLevels = 0,
                                                      bool genMipmaps = true) const = 0;
    virtual std::shared_ptr<ITexture> createTexture3DEmpty(uint32_t width,
                                                           uint32_t height,
                                                           uint32_t depth,
                                                           PixelInternalFormat,
                                                           uint32_t numLevels = 1) const = 0;
    virtual std::shared_ptr<ITexture> createTexture3D(const std::vector<std::shared_ptr<const utils::Image>>&,
                                                      PixelInternalFormat = PixelInternalFormat::Count,
                                                      uint32_t numLevels = 0,
                                                      bool genMipmaps = true) const = 0;
    virtual std::shared_ptr<ITexture> createTextureCubeEmpty(uint32_t width,
                                                             uint32_t height,
                                                             PixelInternalFormat,
                                                             uint32_t numLevels = 1) const = 0;
    virtual std::shared_ptr<ITexture> createTextureCube(const std::vector<std::shared_ptr<const utils::Image>>&,
                                                        PixelInternalFormat = PixelInternalFormat::Count,
                                                        uint32_t numLevels = 0,
                                                        bool genMipmaps = true) const = 0;
    virtual std::shared_ptr<ITexture> createTexture1DArrayEmpty(uint32_t width,
                                                                uint32_t numLayers,
                                                                PixelInternalFormat,
                                                                uint32_t numLevels = 1) const = 0;
    virtual std::shared_ptr<ITexture> createTexture1DArray(const std::vector<std::shared_ptr<const utils::Image>>&,
                                                           PixelInternalFormat = PixelInternalFormat::Count,
                                                           uint32_t numLevels = 0,
                                                           bool genMipmaps = true) const = 0;
    virtual std::shared_ptr<ITexture> createTexture2DArrayEmpty(uint32_t width,
                                                                uint32_t height,
                                                                uint32_t numLayers,
                                                                PixelInternalFormat,
                                                                uint32_t numLevels = 1) const = 0;
    virtual std::shared_ptr<ITexture> createTexture2DArray(const std::vector<std::shared_ptr<const utils::Image>>&,
                                                           PixelInternalFormat = PixelInternalFormat::Count,
                                                           uint32_t numLevels = 0,
                                                           bool genMipmaps = true) const = 0;
    virtual std::shared_ptr<ITexture> createTextureCubeArrayEmpty(uint32_t width,
                                                                  uint32_t height,
                                                                  uint32_t numLayers,
                                                                  PixelInternalFormat,
                                                                  uint32_t numLevels = 1) const = 0;
    virtual std::shared_ptr<ITexture> createTextureCubeArray(const std::vector<std::vector<std::shared_ptr<const utils::Image>>>&, //[layer][face]
                                                             PixelInternalFormat = PixelInternalFormat::Count,
                                                             uint32_t numLevels = 0,
                                                             bool genMipmaps = true) const = 0;
    virtual std::shared_ptr<ITexture> createTextureRectEmpty(uint32_t width,
                                                             uint32_t height,
                                                             core::graphics::PixelInternalFormat) const = 0;
    virtual std::shared_ptr<ITexture> createTextureRect(const std::shared_ptr<const utils::Image>&,
                                                        core::graphics::PixelInternalFormat = core::graphics::PixelInternalFormat::Count) const = 0;
    virtual std::shared_ptr<ITextureHandle> createTextureHandle(const PConstTexture&) const = 0;
    virtual std::shared_ptr<IRenderBuffer> createRenderBuffer(uint32_t width,
                                                              uint32_t height,
                                                              PixelInternalFormat) const = 0;
    virtual std::shared_ptr<IFrameBuffer> createFrameBuffer() const = 0;
    virtual std::shared_ptr<IRenderProgram> createRenderProgram(const std::shared_ptr<utils::Shader> &vertexShader,
                                                                const std::shared_ptr<utils::Shader> &fragmentShader) const = 0;
    virtual std::shared_ptr<IRenderProgram> createRenderProgram(const std::shared_ptr<utils::Shader> &vertexShader,
                                                                const std::shared_ptr<utils::Shader> &geometryShader,
                                                                const std::shared_ptr<utils::Shader> &fragmentShader) const = 0;
    virtual std::shared_ptr<IComputeProgram> createComputeProgram(const std::shared_ptr<utils::Shader> &computeShader) const = 0;

    virtual void compute(
        const glm::uvec3&,
        const std::shared_ptr<IComputeProgram>&,
        const StateSetList&) = 0;

    virtual void drawArrays(
        const glm::uvec4&,
        const std::shared_ptr<graphics::IRenderProgram>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const StateSetList&,
        utils::PrimitiveType,
        size_t first,
        size_t count) = 0;

    virtual void drawElements(
        const glm::uvec4&,
        const std::shared_ptr<graphics::IRenderProgram>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const StateSetList&,
        utils::PrimitiveType,
        size_t count,
        utils::DrawElementsIndexType,
        size_t offset) = 0;

    virtual void multiDrawArrays(
        const glm::uvec4&,
        const std::shared_ptr<graphics::IRenderProgram>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const StateSetList&,
        utils::PrimitiveType,
        const std::vector<size_t>& firsts,
        const std::vector<size_t>& counts) = 0;

    virtual void multiDrawElements(
        const glm::uvec4&,
        const std::shared_ptr<graphics::IRenderProgram>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const StateSetList&,
        utils::PrimitiveType,
        const std::vector<size_t>& counts,
        utils::DrawElementsIndexType,
        const std::vector<size_t>& offsets) = 0;

    virtual void drawElementsBaseVertex(
        const glm::uvec4&,
        const std::shared_ptr<graphics::IRenderProgram>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const StateSetList&,
        utils::PrimitiveType,
        size_t count,
        utils::DrawElementsIndexType,
        size_t offset,
        uint32_t baseVertex) = 0;

    virtual void drawArraysInstanced(
        const glm::uvec4&,
        const std::shared_ptr<graphics::IRenderProgram>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const StateSetList&,
        utils::PrimitiveType,
        size_t first,
        size_t count,
        size_t numInstances) = 0;

    virtual void drawElementsInstanced(
        const glm::uvec4&,
        const std::shared_ptr<graphics::IRenderProgram>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const StateSetList&,
        utils::PrimitiveType,
        size_t count,
        utils::DrawElementsIndexType,
        size_t offset,
        size_t numInstances) = 0;

    virtual void drawArraysInstancedBaseInstance(
        const glm::uvec4&,
        const std::shared_ptr<graphics::IRenderProgram>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const StateSetList&,
        utils::PrimitiveType,
        size_t first,
        size_t count,
        size_t numInstances,
        uint32_t baseInstance) = 0;

    virtual void drawElementsInstancedBaseInstance(
        const glm::uvec4&,
        const std::shared_ptr<graphics::IRenderProgram>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const StateSetList&,
        utils::PrimitiveType,
        size_t count,
        utils::DrawElementsIndexType,
        size_t offset,
        size_t numInstances,
        uint32_t baseInstance) = 0;

    virtual void drawArraysIndirect(
        const glm::uvec4&,
        const std::shared_ptr<graphics::IRenderProgram>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const StateSetList&,
        utils::PrimitiveType,
        const PDrawArraysIndirectCommandsConstBuffer&) = 0;

    virtual void drawElementsIndirect(
        const glm::uvec4&,
        const std::shared_ptr<graphics::IRenderProgram>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const StateSetList&,
        utils::PrimitiveType,
        utils::DrawElementsIndexType,
        const PDrawElementsIndirectCommandConstBuffer&) = 0;

    virtual void multiDrawArraysIndirect(
        const glm::uvec4&,
        const std::shared_ptr<graphics::IRenderProgram>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const StateSetList&,
        utils::PrimitiveType,
        const PDrawArraysIndirectCommandsConstBuffer&) = 0;

    virtual void multiDrawElementsIndirect(
        const glm::uvec4&,
        const std::shared_ptr<graphics::IRenderProgram>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const StateSetList&,
        utils::PrimitiveType,
        utils::DrawElementsIndexType,
        const PDrawElementsIndirectCommandConstBuffer&) = 0;

    virtual void multiDrawArraysIndirectCount(
        const glm::uvec4&,
        const std::shared_ptr<graphics::IRenderProgram>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const StateSetList&,
        utils::PrimitiveType,
        const PDrawArraysIndirectCommandsConstBuffer&) = 0;

    virtual void multiDrawElementsIndirectCount(
        const glm::uvec4&,
        const std::shared_ptr<graphics::IRenderProgram>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const StateSetList&,
        utils::PrimitiveType,
        utils::DrawElementsIndexType,
        const PDrawElementsIndirectCommandConstBuffer&) = 0;

    virtual void clearRenderData() = 0;
    virtual void addRenderData(const std::shared_ptr<core::graphics::IRenderProgram>&,
                               const std::shared_ptr<Drawable>&,
                               const glm::mat4x4& = glm::mat4x4(1.f)) = 0;
    virtual void render(const std::shared_ptr<IFrameBuffer>&,
                        const glm::uvec4 &viewport,
                        const std::shared_ptr<const StateSet>&) = 0;

protected:
    virtual bool doMakeCurrent() = 0;
    virtual bool doDoneCurrent() = 0;

    std::unique_ptr<RendererBasePrivate> m_;
};

class CORE_SHARED_EXPORT BufferRange final
{
    NONCOPYBLE(BufferRange)
public:
    ~BufferRange();

    std::shared_ptr<const IBuffer> buffer() const;
    size_t offset() const;
    size_t size() const;

    static std::shared_ptr<BufferRange> create(
        const std::shared_ptr<const IBuffer>&,
        size_t offset = 0u,
        size_t size = static_cast<size_t>(-1));

private:
    BufferRange(const std::shared_ptr<const IBuffer>&, size_t, size_t);

    std::shared_ptr<const IBuffer> m_buffer;
    size_t m_offset;
    size_t m_size;
};

class CORE_SHARED_EXPORT Image final
{
public:
    ~Image();

    ENUMCLASS(DataAccess, uint8_t, ReadOnly, WriteOnly, ReadWrite)

    DataAccess access() const;
    PConstTexture texture() const;
    uint32_t mipmapLevel() const;

    static const SupportedImageFormats& supportedImageFormats();
    static std::shared_ptr<Image> create(DataAccess, const PConstTexture&, uint32_t mipmapLevel = 0u);

private:
    Image(DataAccess, const PConstTexture&, uint32_t mipmapLevel);

    DataAccess m_access;
    uint32_t m_mipmapLevel;
    PConstTexture m_texture;
};

class CORE_SHARED_EXPORT VAOMesh final
{
    NONCOPYBLE(VAOMesh)
public:
    ~VAOMesh();

    std::shared_ptr<IVertexArray> vao();
    std::shared_ptr<const IVertexArray> vao() const;

    void addPrimitiveSet(const std::shared_ptr<utils::PrimitiveSet>&);
    void removePrimitiveSet(const std::shared_ptr<utils::PrimitiveSet>&);
    const std::unordered_set<std::shared_ptr<utils::PrimitiveSet>>& primitiveSets() const;

    static std::shared_ptr<VAOMesh> create(
        const std::shared_ptr<const utils::Mesh>& = nullptr,
        bool uniteVertexBuffers = true);
private:
    VAOMesh(const std::shared_ptr<IVertexArray>&);

    std::shared_ptr<IVertexArray> m_vao;
    std::unordered_set<std::shared_ptr<utils::PrimitiveSet>> m_primitiveSets;
};

template <typename T>
class StructBuffer
{
    NONCOPYBLE(StructBuffer)
public:
    using value_type = T;
    static constexpr size_t sizeofT() { return sizeof(value_type); }

    ~StructBuffer() = default;

    std::shared_ptr<const IStaticBuffer> buffer() const { return m_buffer; }

    value_type get() const
    {
        return *reinterpret_cast<value_type*>(m_buffer->map(IBuffer::MapAccess::ReadOnly)->get());
    }

    void set(const value_type& value)
    {
        *reinterpret_cast<value_type*>(m_buffer->map(IBuffer::MapAccess::WriteOnly)->get()) = value;
    }

    static std::shared_ptr<StructBuffer<value_type>> create(const value_type& value = value_type())
    {
        auto currentContext = RendererBase::current();
        if (!currentContext)
        {
            LOG_CRITICAL << "No current context";
            return nullptr;
        }

        return std::shared_ptr<StructBuffer<value_type>>(
            new StructBuffer<value_type>(currentContext->createStaticBuffer(sizeofT(), &value)));
    }

private:
    StructBuffer(const std::shared_ptr<IStaticBuffer>& buffer)
        : m_buffer(buffer)
    {
    }

    std::shared_ptr<IStaticBuffer> m_buffer;
};

template <typename T, typename ReservedType = void>
class VectorBuffer final
{
    NONCOPYBLE(VectorBuffer)
public:
    using value_type = T;
    static constexpr size_t sizeofT() { return sizeof(value_type); }

    using reserved_type = ReservedType;
    static constexpr size_t sizeofReservedType() {
        if constexpr (std::is_same_v<reserved_type, void>) return 0u;
        else return sizeof(reserved_type);
    }

    ~VectorBuffer() = default;

    std::shared_ptr<const IDynamicBuffer> buffer() const { return m_buffer; }

    template <typename U = reserved_type, typename std::enable_if<!std::is_void<U>::value>::type* = nullptr>
    reserved_type reservedData() const {
        return *reinterpret_cast<reserved_type*>(m_buffer->map(IBuffer::MapAccess::ReadOnly, 0u, sizeofReservedType())->get());
    }

    template <typename U = reserved_type, typename std::enable_if<!std::is_void<U>::value>::type* = nullptr>
    void setReservedData(const U& value) {
        *reinterpret_cast<reserved_type*>(m_buffer->map(IBuffer::MapAccess::WriteOnly, 0u, sizeofReservedType())->get()) = value;
    }

    bool isEmpty() const { return size() == 0u; }
    size_t size() const { return (m_buffer->size() - sizeofReservedType()) / sizeofT(); }

    size_t capacity() const { return (m_buffer->capacity() - sizeofReservedType()) / sizeofT(); }
    void reserve(size_t value) { m_buffer->reserve(value * sizeofT() + sizeofReservedType()); }
    void shrinkToFit() { m_buffer->shrinkToFit(); }

    void clear() { resize(0u); }
    void insert(size_t index, const value_type* data, size_t count) {
        m_buffer->insert(index * sizeofT() + sizeofReservedType(), data, count * sizeofT()); }
    void insert(size_t index, std::initializer_list<value_type> l) { insert(index, l.begin(), l.size()); }
    void erase(size_t index, size_t count) { m_buffer->erase(index * sizeofT() + sizeofReservedType(), count * sizeofT()); }
    void resize(size_t count) { m_buffer->resize(count * sizeofT() + sizeofReservedType()); }

    void pushBack(const value_type& value) { insert(size(), &value, 1u); }
    void set(size_t index, const value_type& value)
    {
        if (index >= size())
            LOG_CRITICAL << "Index is out of range";

        *reinterpret_cast<value_type*>(m_buffer->map(
            IBuffer::MapAccess::WriteOnly,
            index * sizeofT() + sizeofReservedType(),
            sizeofT())->get()) = value;
    }
    value_type get(size_t index) const
    {
        if (index >= size())
            LOG_CRITICAL << "Index is out of range";

        return *reinterpret_cast<value_type*>(m_buffer->map(
            IBuffer::MapAccess::ReadOnly,
            index * sizeofT() + sizeofReservedType(),
            sizeofT())->get());
    }

    static std::shared_ptr<VectorBuffer<value_type, reserved_type>> create(std::initializer_list<value_type> l = {})
    {
        auto currentContext = RendererBase::current();
        if (!currentContext)
        {
            LOG_CRITICAL << "No current context";
            return nullptr;
        }

        return std::shared_ptr<VectorBuffer<value_type, reserved_type>>(new VectorBuffer<value_type, reserved_type>(
            currentContext->createDynamicBuffer(), l.begin(), l.size()));
    }

    template <typename U = reserved_type, typename std::enable_if<!std::is_void<U>::value>::type* = nullptr>
    static std::shared_ptr<VectorBuffer<value_type, U>> create(const U& r = U(), std::initializer_list<value_type> l = {})
    {
        auto result = create(l);
        result->setReservedData(r);
        return result;
    }

private:
    VectorBuffer(const std::shared_ptr<IDynamicBuffer>& buffer, const value_type* data, size_t count)
        : m_buffer(buffer)
    {
        clear();
        insert(0u, data, count);
    }

    std::shared_ptr<IDynamicBuffer> m_buffer;
};

inline UniformType IProgram::uniformTypeByTextureType(TextureType textureType)
{
    static const std::unordered_map<TextureType, UniformType> s_table {
            { TextureType::Type1D, UniformType::Sampler1D },
            { TextureType::Type2D, UniformType::Sampler2D },
            { TextureType::Type3D, UniformType::Sampler3D },
            { TextureType::TypeCube, UniformType::SamplerCube },
            { TextureType::Type1DArray, UniformType::Sampler1DArray },
            { TextureType::Type2DArray, UniformType::Sampler2DArray },
            { TextureType::TypeCubeArray, UniformType::SamplerCubeArray },
            { TextureType::TypeRect, UniformType::SamplerRect },
        };

        auto it = s_table.find(textureType);
        return (it == s_table.end()) ? UniformType::Count : it->second;
}

inline UniformType IProgram::uniformTypeByImageTextureType(TextureType textureType)
{
    static const std::unordered_map<TextureType, UniformType> s_table {
            { TextureType::Type1D, UniformType::Image1D },
            { TextureType::Type2D, UniformType::Image2D },
            { TextureType::Type3D, UniformType::Image3D },
            { TextureType::TypeCube, UniformType::ImageCube },
            { TextureType::Type1DArray, UniformType::Image1DArray },
            { TextureType::Type2DArray, UniformType::Image2DArray },
            { TextureType::TypeCubeArray, UniformType::ImageCubeArray },
            { TextureType::TypeRect, UniformType::ImageRect },
        };

        auto it = s_table.find(textureType);
        return (it == s_table.end()) ? UniformType::Count : it->second;
}

}
}
}

#endif // CORE_IGRAPHICSRENDERER_H

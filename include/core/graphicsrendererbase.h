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
          Undefined,
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

static core::graphics::PixelInternalFormat pixelNumComponentsAndPixelComponentTypeToPixelInternalFormat(uint32_t, utils::PixelComponentType);

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
          Undefined,
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
    utils::VertexAttribute id;
    uint16_t index;
    int32_t location;
    uint16_t numComponents;
    utils::VertexComponentType componentType;
};

struct UniformInfo
{
    uint16_t id;
    uint16_t index;
    int32_t location;
    UniformType type;
};

struct SSBOVariableInfo
{
    uint16_t index;
    //SSBOVariableType type;
    uint16_t offset;
};

struct SSBOInfo
{
    uint16_t id;
    uint16_t index;
    std::vector<SSBOVariableInfo> variables;
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
    class MappedData {
    public:
        virtual ~MappedData() = default; // mapped data should be unmapped in destructor's implementation
        virtual const uint8_t *get() const = 0;
        virtual uint8_t *get() = 0;
    };

    virtual ~IBuffer() = default;

    virtual size_t size() const = 0;
    virtual void resize(size_t size, const void *data) = 0;

    ENUMCLASS(MapAccess, uint8_t, ReadOnly, WriteOnly, ReadWrite)
    virtual std::unique_ptr<const MappedData> map(MapAccess access, size_t offset = 0u, size_t size = 0u) const = 0;
    virtual std::unique_ptr<MappedData> map(MapAccess access, size_t offset = 0u, size_t size = 0u) = 0;
};

class IBufferRange
{
public:
    virtual ~IBufferRange() = default;

    virtual std::shared_ptr<const IBuffer> buffer() const = 0;
    virtual std::shared_ptr<IBuffer> buffer() = 0;

    virtual size_t offset() const = 0;
    virtual void setOffset(size_t) = 0;

    virtual size_t size() const = 0;
    virtual void setSize(size_t) = 0;
};

class IVertexArray
{
public:
    virtual ~IVertexArray() = default;

    virtual uint32_t attachVertexBuffer(std::shared_ptr<IBuffer> buffer, size_t offset, uint32_t stride) = 0; // returns bindingIndex
    virtual void detachVertexBuffer(uint32_t bindingIndex) = 0;
    virtual std::shared_ptr<const IBuffer> vertexBuffer(uint32_t bindingIndex) const = 0;
    virtual size_t vertexBufferOffset(uint32_t bindingIndex) const = 0;
    virtual uint32_t vertexBufferStride(uint32_t bindingIndex) const = 0;

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

    virtual void addPrimitiveSet(std::shared_ptr<utils::PrimitiveSet>) = 0;
    virtual void removePrimitiveSet(std::shared_ptr<utils::PrimitiveSet>) = 0;
    virtual const std::unordered_set<std::shared_ptr<utils::PrimitiveSet>> &primitiveSets() const = 0;
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

class IImage
{
public:
    virtual ~IImage() = default;

    ENUMCLASS(DataAccess, uint8_t, ReadOnly, WriteOnly, ReadWrite)
    virtual DataAccess access() const = 0;
    virtual void setAccess(DataAccess) = 0;

    virtual uint32_t mipmapLevel() const = 0;
    virtual PixelInternalFormat format() const = 0;
    virtual PConstTexture texture() const = 0;
    virtual void setTexture(const PConstTexture&, uint32_t) = 0;
};

class IRenderBuffer : public ISurface
{
public:
};

class IFrameBuffer
{
public:
    struct AttachmentInfo
    {
        AttachmentInfo(std::shared_ptr<const ISurface> _renderSurface = nullptr, uint32_t _level = 0u, uint32_t _layer = 0u)
            : renderSurface(_renderSurface), level(_level), layer(_layer)
        {}

        std::shared_ptr<const ISurface> renderSurface;
        uint32_t level;
        uint32_t layer;
    };

    using Attachments = std::unordered_map<FrameBufferAttachment, AttachmentInfo>;

    virtual ~IFrameBuffer() = default;

    virtual bool isComplete() const = 0;

    virtual const Attachments &attachments() const = 0;
    virtual bool attachment(FrameBufferAttachment, AttachmentInfo&) const = 0;
    virtual void attach(FrameBufferAttachment, std::shared_ptr<const ISurface>, uint32_t level = 0u) = 0;
    virtual void attachLayer(FrameBufferAttachment, std::shared_ptr<const ITexture>, uint32_t level = 0u, uint32_t layer = 0u) = 0;
    virtual void detach(FrameBufferAttachment) = 0;

    virtual const FrameBufferClearColor &clearColor(uint32_t) const = 0;
    virtual void setClearColor(uint32_t, const glm::vec4&) = 0;
    virtual void setClearColor(uint32_t, const glm::i32vec4&) = 0;
    virtual void setClearColor(uint32_t, const glm::u32vec4&) = 0;

    virtual float clearDepth() const = 0;
    virtual int32_t clearStencil() const = 0;
    virtual void setClearDepthStencil(float, uint8_t) = 0;

    virtual const std::unordered_set<FrameBufferAttachment> &clearMask() const = 0;
    virtual void setClearMask(const std::unordered_set<FrameBufferAttachment>&) = 0;

    virtual void setDrawBuffers(const std::vector<FrameBufferAttachment>&) = 0; 

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
    virtual BlendEquation blendColorEquation(uint32_t) = 0;
    virtual BlendEquation blendAlphaEquation(uint32_t) = 0;
    virtual void setBlendEquation(uint32_t, BlendEquation, BlendEquation) = 0;
    virtual BlendFactor blendColorSourceFactor(uint32_t) = 0;
    virtual BlendFactor blendAlphaSourceFactor(uint32_t) = 0;
    virtual BlendFactor blendColorDestinationFactor(uint32_t) = 0;
    virtual BlendFactor blendAlphaDestinationFactor(uint32_t) = 0;
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

    virtual int32_t uniformLocationByName(const std::string&) const = 0;

    virtual const std::vector<UniformInfo> &uniformsInfo() const = 0;
    virtual const std::vector<SSBOInfo> &SSBOsInfo() const = 0;

    virtual std::string uniformNameByIndex(uint16_t) const = 0;
    virtual std::string SSBOVariableNameByIndex(uint16_t) const = 0;
    virtual std::string SSBONameByIndex(uint16_t) const = 0;

    static UniformType uniformTypeByTextureType(TextureType);
    static UniformType uniformTypeByImageTextureType(TextureType);
};

class IRenderProgram : public virtual IProgram
{
public:
    virtual int32_t attributeLocationByName(const std::string&) const = 0;
    virtual const std::vector<AttributeInfo> &attributesInfo() const = 0;
    virtual std::string attributeNameByIndex(uint16_t) const = 0;
};

class IComputeProgram : public virtual IProgram
{
public:
    virtual glm::uvec3 workGroupSize() const = 0;
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

    virtual std::shared_ptr<IGraphicsWidget> widget() = 0;
    virtual std::shared_ptr<const IGraphicsWidget> widget() const = 0;

    virtual void blitFrameBuffer(std::shared_ptr<const IFrameBuffer> src,
                                 std::shared_ptr<IFrameBuffer> dst,
                                 const glm::uvec4 &srcViewport,
                                 const glm::uvec4 &dstViewport,
                                 bool colorMsk, bool depthMask, bool stencilMask,
                                 bool linearFilter = false) = 0;

    virtual bool registerVertexAttribute(const std::string&, utils::VertexAttribute) = 0;
    virtual bool unregisterVertexAttribute(const std::string&) = 0;
    virtual utils::VertexAttribute vertexAttributeByName(const std::string&) const = 0;

    virtual bool registerUniformId(const std::string&, uint16_t) = 0;
    virtual bool unregisterUniformId(const std::string&) = 0;
    virtual uint16_t uniformIdByName(const std::string&) const = 0;

    virtual bool registerSSBOId(const std::string&, uint16_t) = 0;
    virtual bool unregisterSSBOId(const std::string&) = 0;
    virtual uint16_t SSBOIdByName(const std::string&) const = 0;

    virtual std::shared_ptr<IBuffer> createBuffer(size_t size = 0u, const void *data = nullptr) const = 0;
    virtual std::shared_ptr<IBufferRange> createBufferRange(const std::shared_ptr<IBuffer>&,
                                                            size_t,
                                                            size_t = static_cast<size_t>(-1)) const = 0;
    virtual std::shared_ptr<IVertexArray> createVertexArray(const std::shared_ptr<utils::Mesh>& = nullptr,
                                                            bool uniteBuffers = true) const = 0;
    virtual std::shared_ptr<ITexture> createTexture1DEmpty(uint32_t width,
                                                           PixelInternalFormat,
                                                           uint32_t numLevels = 1) const = 0;
    virtual std::shared_ptr<ITexture> createTexture1D(const std::shared_ptr<utils::Image>&,
                                                      PixelInternalFormat = PixelInternalFormat::Undefined,
                                                      uint32_t numLevels = 0,
                                                      bool genMipmaps = true) const = 0;
    virtual std::shared_ptr<ITexture> createTexture2DEmpty(uint32_t width,
                                                           uint32_t height,
                                                           PixelInternalFormat,
                                                           uint32_t numLevels = 1) const = 0;
    virtual std::shared_ptr<ITexture> createTexture2D(const std::shared_ptr<utils::Image>&,
                                                      PixelInternalFormat = PixelInternalFormat::Undefined,
                                                      uint32_t numLevels = 0,
                                                      bool genMipmaps = true) const = 0;
    virtual std::shared_ptr<ITexture> createTexture3DEmpty(uint32_t width,
                                                           uint32_t height,
                                                           uint32_t depth,
                                                           PixelInternalFormat,
                                                           uint32_t numLevels = 1) const = 0;
    virtual std::shared_ptr<ITexture> createTexture3D(const std::vector<std::shared_ptr<utils::Image>>&,
                                                      PixelInternalFormat = PixelInternalFormat::Undefined,
                                                      uint32_t numLevels = 0,
                                                      bool genMipmaps = true) const = 0;
    virtual std::shared_ptr<ITexture> createTextureCubeEmpty(uint32_t width,
                                                             uint32_t height,
                                                             PixelInternalFormat,
                                                             uint32_t numLevels = 1) const = 0;
    virtual std::shared_ptr<ITexture> createTextureCube(const std::vector<std::shared_ptr<utils::Image>>&,
                                                        PixelInternalFormat = PixelInternalFormat::Undefined,
                                                        uint32_t numLevels = 0,
                                                        bool genMipmaps = true) const = 0;
    virtual std::shared_ptr<ITexture> createTexture1DArrayEmpty(uint32_t width,
                                                                uint32_t numLayers,
                                                                PixelInternalFormat,
                                                                uint32_t numLevels = 1) const = 0;
    virtual std::shared_ptr<ITexture> createTexture1DArray(const std::vector<std::shared_ptr<utils::Image>>&,
                                                           PixelInternalFormat = PixelInternalFormat::Undefined,
                                                           uint32_t numLevels = 0,
                                                           bool genMipmaps = true) const = 0;
    virtual std::shared_ptr<ITexture> createTexture2DArrayEmpty(uint32_t width,
                                                                uint32_t height,
                                                                uint32_t numLayers,
                                                                PixelInternalFormat,
                                                                uint32_t numLevels = 1) const = 0;
    virtual std::shared_ptr<ITexture> createTexture2DArray(const std::vector<std::shared_ptr<utils::Image>>&,
                                                           PixelInternalFormat = PixelInternalFormat::Undefined,
                                                           uint32_t numLevels = 0,
                                                           bool genMipmaps = true) const = 0;
    virtual std::shared_ptr<ITexture> createTextureCubeArrayEmpty(uint32_t width,
                                                                  uint32_t height,
                                                                  uint32_t numLayers,
                                                                  PixelInternalFormat,
                                                                  uint32_t numLevels = 1) const = 0;
    virtual std::shared_ptr<ITexture> createTextureCubeArray(const std::vector<std::vector<std::shared_ptr<utils::Image>>>&, //[layer][face]
                                                             PixelInternalFormat = PixelInternalFormat::Undefined,
                                                             uint32_t numLevels = 0,
                                                             bool genMipmaps = true) const = 0;
    virtual std::shared_ptr<ITexture> createTextureRectEmpty(uint32_t width,
                                                             uint32_t height,
                                                             core::graphics::PixelInternalFormat) const = 0;
    virtual std::shared_ptr<ITexture> createTextureRect(const std::shared_ptr<utils::Image>&,
                                                        core::graphics::PixelInternalFormat = core::graphics::PixelInternalFormat::Undefined) const = 0;
    virtual std::shared_ptr<IImage> createImage(IImage::DataAccess,
                                                const PConstTexture&,
                                                uint32_t level = 0u) const = 0;
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

    virtual const SupportedImageFormats &supportedImageFormats() const = 0;

    virtual const glm::uvec2 &screenSize() const = 0;
    virtual void resize(const glm::uvec2&) = 0;

    virtual void clearRenderData() = 0;
    virtual void addRenderData(const std::shared_ptr<core::graphics::IRenderProgram>&,
                               const std::shared_ptr<const Drawable>&,
                               const glm::mat4x4& = glm::mat4x4(1.f)) = 0;
    virtual void render(const std::shared_ptr<IFrameBuffer>&,
                        const glm::uvec4 &viewport,
                        const glm::mat4x4 &viewMatrix,
                        const glm::mat4x4 &projectionMatrix,
                        const PConstStateSet&) = 0;
    virtual void compute(const std::shared_ptr<IComputeProgram>&,
                         const glm::uvec3&,
                         const PConstStateSet&) = 0;

protected:
    virtual bool doMakeCurrent() = 0;
    virtual bool doDoneCurrent() = 0;

    std::unique_ptr<RendererBasePrivate> m_;
};

inline core::graphics::PixelInternalFormat pixelNumComponentsAndPixelComponentTypeToPixelInternalFormat(uint32_t numComponents,
                                                                                                        utils::PixelComponentType type)
{
    PixelInternalFormat result = core::graphics::PixelInternalFormat::Undefined;

    switch (type)
    {
    case utils::PixelComponentType::Uint8:
    {
        switch (numComponents)
        {
        case 1: { result = core::graphics::PixelInternalFormat::R8; break; }
        case 2: { result = core::graphics::PixelInternalFormat::RG8; break; }
        case 3: { result = core::graphics::PixelInternalFormat::RGB8; break; }
        case 4: { result = core::graphics::PixelInternalFormat::RGBA8; break; }
        default: { break; }
        }
    break;
    }
        case utils::PixelComponentType::Uint16:
        {
        switch (numComponents)
        {
        case 1: { result = core::graphics::PixelInternalFormat::R16; break; }
        case 2: { result = core::graphics::PixelInternalFormat::RG16; break; }
        case 3: { result = core::graphics::PixelInternalFormat::RGB16; break; }
        case 4: { result = core::graphics::PixelInternalFormat::RGBA16; break; }
        default: { break; }
        }
        break;
        }
    case utils::PixelComponentType::Single:
    {
        switch (numComponents)
        {
        case 1: { result = core::graphics::PixelInternalFormat::R16F; break; }
        case 2: { result = core::graphics::PixelInternalFormat::RG16F; break; }
        case 3: { result = core::graphics::PixelInternalFormat::RGB16F; break; }
        case 4: { result = core::graphics::PixelInternalFormat::RGBA16F; break; }
        default: { break; }
        }
    break;
    }
    default: { break; }
    }

    return result;
}

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
        return (it == s_table.end()) ? UniformType::Undefined : it->second;
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
        return (it == s_table.end()) ? UniformType::Undefined : it->second;
}

}
}
}

#endif // CORE_IGRAPHICSRENDERER_H

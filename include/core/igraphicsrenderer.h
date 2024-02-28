#ifndef IGRAPHICSRENDERER_H
#define IGRAPHICSRENDERER_H

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

#include <core/forwarddecl.h>
#include <core/inamedobject.h>

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

ENUMCLASS(UniformId, uint16_t,
          Undefined,
          ViewportSize,
          ModelMatrix,
          NormalMatrix,
          ViewMatrix,
          ViewMatrixInverse,
          ProjectionMatrix,
          ProjectionMatrixInverse,
          ViewProjectionMatrix,
          ViewProjectionMatrixInverse,
          ModelViewMatrix,
          NormalViewMatrix,
          ModelViewProjectionMatrix,
          ModelPosition,
          ModelXDirection,
          ModelYDirection,
          ModelZDirection,
          ViewPosition,
          ViewXDirection,
          ViewYDirection,
          ViewZDirection,
          BackgroundColorMap,
          BackgroundBaseColor,
          BackgroundRoughness,
          GBufferColor0Map,
          GBufferColor1Map,
          GBufferColor2Map,
          GBufferDepthMap,
          OITDepthImage,
          OITIndicesImage,
          OITNodesCounter,
          LightBufferColorMap,
          FinalBufferColorMap,
          AlphaCutoff,
          ORMSwizzleMask,
          BaseColor,
          BaseColorMap,
          Metalness,
          MetalnessMap,
          Roughness,
          RoughnessMap,
          NormalMap,
          NormalMapScale,
          OcclusionMap,
          OcclusionMapStrength,
          LightColor,
          LightRadiuses,
          LightCosHalfAngles,
          IBLBRDFLutMap,
          IBLDiffuseMap,
          IBLSpecularMap,
          IBLContribution)

ENUMCLASS(UniformType, uint16_t,
          Undefined,
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

ENUMCLASS(SSBOId, uint16_t,
          Undefined,
          BonesBuffer,
          OITNodes)

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
    UniformId id;
    uint16_t index;
    int32_t location;
    UniformType type;
};

struct SSBOInfo
{
    SSBOId id;
    uint16_t index;
};

ENUMCLASS(PBRComponent, uint16_t,
          BaseColor,
          BaseColorMap,
          Metalness,
          MetalnessMap,
          Roughness,
          RoughnessMap,
          NormalMap,
          NormalMapScale,
          OcclusionMap,
          OcclusionMapStrength)

ENUMCLASS(LightComponent, uint16_t,
          None)

ENUMCLASS(BackgroundComponent, uint16_t,
          ColorMap,
          BaseColor,
          Roughness)

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
    virtual size_t size() const = 0;
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
};

class ITexture : public ISurface
{
public:
    virtual TextureType type() const = 0;

    virtual glm::uvec3 mipmapSize(uint32_t level = 0) const = 0;
    virtual uint32_t numMipmapLevels() const = 0;

    virtual void setSubImage(uint32_t level,
                             const glm::uvec3 &offset,
                             const glm::uvec3 &size,
                             uint32_t numComponents,
                             utils::PixelComponentType type,
                             const void *data) = 0;

    virtual void generateMipmaps() = 0;
    virtual void setBorderColor(const glm::vec4&) = 0;
    virtual void setWrapMode(TextureWrapMode) = 0;
    virtual void setFilterMode(TextureFilterMode) = 0;
    virtual void setSwizzleMask(const TextureSwizzleMask&) = 0;

};

class IImage
{
public:
    virtual ~IImage() = default;

    virtual std::shared_ptr<const ITexture> texture() const = 0;
    virtual std::shared_ptr<ITexture> texture() = 0;

    ENUMCLASS(DataAccess, uint8_t, ReadOnly, WriteOnly, ReadWrite)
    virtual DataAccess access() const = 0;

    virtual uint32_t mipmapLevel() const = 0;
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
    virtual void attach(FrameBufferAttachment, std::shared_ptr<const ISurface>, uint32_t level = 0u, uint32_t layer = 0u) = 0;
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
    virtual std::string SSBONameByIndex(uint16_t) const = 0;

    static UniformType uniformTypeByTextureType(TextureType);
    static UniformType uniformTypeByImageTextureType(TextureType);

    static UniformId UniformIdByName(const std::string&);
    static SSBOId SSBOIdByName(const std::string&);

    static UniformId uniformIdByPBRComponent(PBRComponent);
    static UniformId uniformIdByLightComponent(LightComponent);
    static UniformId uniformIdByBackgroundComponent(BackgroundComponent);
};

class IRenderProgram : public virtual IProgram
{
public:
    virtual int32_t attributeLocationByName(const std::string&) const = 0;
    virtual const std::vector<AttributeInfo> &attributesInfo() const = 0;
    virtual std::string attributeNameByIndex(uint16_t) const = 0;

    static utils::VertexComponentType attributeVertexComponentTypeByAttributeId(utils::VertexAttribute);
    static utils::VertexAttribute vertexAttributeByName(const std::string&);
};

class IComputeProgram : public virtual IProgram
{
public:
    virtual glm::uvec3 workGroupSize() const = 0;
};

class IRenderer : public INamedObject
{
public:
    virtual std::shared_ptr<IFrameBuffer> defaultFrameBuffer() = 0;
    virtual std::shared_ptr<const IFrameBuffer> defaultFrameBuffer() const = 0;

    virtual void blitFrameBuffer(std::shared_ptr<const IFrameBuffer> src,
                                 std::shared_ptr<IFrameBuffer> dst,
                                 const glm::uvec4 &srcViewport,
                                 const glm::uvec4 &dstViewport,
                                 bool colorMsk, bool depthMask, bool stencilMask,
                                 bool linearFilter = false) = 0;

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
    virtual std::shared_ptr<IImage> createImage(const std::shared_ptr<ITexture>&,
                                                IImage::DataAccess,
                                                uint32_t level = 0u) const = 0;
    virtual std::shared_ptr<IRenderBuffer> createRenderBuffer(uint32_t width,
                                                              uint32_t height,
                                                              PixelInternalFormat) const = 0;
    virtual std::shared_ptr<IFrameBuffer> createFrameBuffer() const = 0;
    virtual std::shared_ptr<IRenderProgram> createRenderProgram(const std::shared_ptr<utils::Shader> &vertexShader,
                                                                const std::shared_ptr<utils::Shader> &fragmentShader) const = 0;
    virtual std::shared_ptr<IComputeProgram> createComputeProgram(const std::shared_ptr<utils::Shader> &computeShader) const = 0;

    virtual void resize(uint32_t, uint32_t) = 0;
    virtual const glm::uvec2 &viewportSize() const = 0;

    virtual void clearRenderData() = 0;
    virtual void addRenderData(const std::shared_ptr<core::graphics::IRenderProgram>&,
                               const std::shared_ptr<const Drawable>&,
                               const glm::mat4x4& = glm::mat4x4(1.f)) = 0;
    virtual void render(const std::shared_ptr<IFrameBuffer>&, const RenderInfo&, const glm::uvec4&) = 0;
    virtual void compute(const std::shared_ptr<IComputeProgram>&, const RenderInfo&, const glm::uvec3&) = 0;
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

inline utils::VertexComponentType IRenderProgram::attributeVertexComponentTypeByAttributeId(utils::VertexAttribute vertexAttribute)
{
    static const std::array<utils::VertexComponentType, utils::numElementsVertexAttribute()> s_table {
        utils::VertexComponentType::Single,
        utils::VertexComponentType::Single,
        utils::VertexComponentType::Single,
        utils::VertexComponentType::Uint32,
        utils::VertexComponentType::Single,
        utils::VertexComponentType::Single,
        utils::VertexComponentType::Single
    };

    return s_table[utils::castFromVertexAttribute(vertexAttribute)];
}

inline utils::VertexAttribute IRenderProgram::vertexAttributeByName(const std::string &name)
{
    static const std::unordered_map<std::string, utils::VertexAttribute> s_table {
        { "a_position", utils::VertexAttribute::Position },
        { "a_normal", utils::VertexAttribute::Normal },
        { "a_texCoord", utils::VertexAttribute::TexCoords },
        { "a_bonesIDs", utils::VertexAttribute::BonesIDs },
        { "a_bonesWeights", utils::VertexAttribute::BonesWeights },
        { "a_tangent", utils::VertexAttribute::Tangent },
        { "a_color", utils::VertexAttribute::Color },
    };

    auto it = s_table.find(name);
    return (it == s_table.end()) ? utils::VertexAttribute::Count : it->second;
}

inline UniformId IProgram::UniformIdByName(const std::string &name)
{
    static const std::unordered_map<std::string, UniformId> s_table {
        { "u_viewportSize", UniformId::ViewportSize },
        { "u_modelMatrix", UniformId::ModelMatrix },
        { "u_normalMatrix", UniformId::NormalMatrix },
        { "u_viewMatrix", UniformId::ViewMatrix },
        { "u_viewMatrixInverse", UniformId::ViewMatrixInverse },
        { "u_projectionMatrix", UniformId::ProjectionMatrix },
        { "u_projectionMatrixInverse", UniformId::ProjectionMatrixInverse },
        { "u_viewProjectionMatrix", UniformId::ViewProjectionMatrix },
        { "u_viewProjectionMatrixInverse", UniformId::ViewProjectionMatrixInverse },
        { "u_modelViewMatrix", UniformId::ModelViewMatrix },
        { "u_normalViewMatrix", UniformId::NormalViewMatrix },
        { "u_modelViewProjectionMatrix", UniformId::ModelViewProjectionMatrix },
        { "u_modelPosition", UniformId::ModelPosition },
        { "u_modelXDirection", UniformId::ModelXDirection },
        { "u_modelYDirection", UniformId::ModelYDirection },
        { "u_modelZDirection", UniformId::ModelZDirection },
        { "u_viewPosition", UniformId::ViewPosition },
        { "u_viewXDirection", UniformId::ViewXDirection },
        { "u_viewYDirection", UniformId::ViewYDirection },
        { "u_viewZDirection", UniformId::ViewZDirection },
        { "u_backgroundColorMap", UniformId::BackgroundColorMap },
        { "u_backgroundBaseColor", UniformId::BackgroundBaseColor },
        { "u_backgroundRoughness", UniformId::BackgroundRoughness },
        { "u_GBufferColor0Map", UniformId::GBufferColor0Map },
        { "u_GBufferColor1Map", UniformId::GBufferColor1Map },
        { "u_GBufferColor2Map", UniformId::GBufferColor2Map },
        { "u_GBufferDepthMap", UniformId::GBufferDepthMap },
        { "u_OITDepthImage", UniformId::OITDepthImage },
        { "u_OITIndicesImage", UniformId::OITIndicesImage },
        { "ssbo_OITNodesCounter", UniformId::OITNodesCounter },
        { "u_lightBufferColorMap", UniformId::LightBufferColorMap },
        { "u_finalBufferColorMap", UniformId::FinalBufferColorMap },
        { "u_alphaCutoff", UniformId::AlphaCutoff },
        { "u_ORMSwizzleMask", UniformId::ORMSwizzleMask },
        { "u_baseColor", UniformId::BaseColor },
        { "u_baseColorMap", UniformId::BaseColorMap },
        { "u_metalness", UniformId::Metalness },
        { "u_metalnessMap", UniformId::MetalnessMap },
        { "u_roughness", UniformId::Roughness },
        { "u_roughnessMap", UniformId::RoughnessMap },
        { "u_normalMap", UniformId::NormalMap },
        { "u_normalMapScale", UniformId::NormalMapScale },
        { "u_occlusionMap", UniformId::OcclusionMap },
        { "u_occlusionMapStrength", UniformId::OcclusionMapStrength },
        { "u_lightColor", UniformId::LightColor },
        { "u_lightRadiuses", UniformId::LightRadiuses },
        { "u_lightCosHalfAngles", UniformId::LightCosHalfAngles },
        { "u_IBLBRDFLutMap", UniformId::IBLBRDFLutMap },
        { "u_IBLDiffuseMap", UniformId::IBLDiffuseMap },
        { "u_IBLSpecularMap", UniformId::IBLSpecularMap },
        { "u_IBLContribution", UniformId::IBLContribution }
    };

    auto it = s_table.find(name);
    return (it == s_table.end()) ? UniformId::Undefined : it->second;
}

inline SSBOId IProgram::SSBOIdByName(const std::string &name)
{
    static const std::unordered_map<std::string, SSBOId> s_table {
        { "ssbo_bonesBuffer", SSBOId::BonesBuffer },
        { "ssbo_OITNodes", SSBOId::OITNodes }
    };

    auto it = s_table.find(name);
    return (it == s_table.end()) ? SSBOId::Undefined : it->second;
}

inline UniformId IProgram::uniformIdByPBRComponent(PBRComponent value)
{
    static const std::unordered_map<PBRComponent, UniformId> s_table {
        { PBRComponent::BaseColor, UniformId::BaseColor },
        { PBRComponent::BaseColorMap, UniformId::BaseColorMap },
        { PBRComponent::Metalness, UniformId::Metalness },
        { PBRComponent::MetalnessMap, UniformId::MetalnessMap },
        { PBRComponent::Roughness, UniformId::Roughness },
        { PBRComponent::RoughnessMap, UniformId::RoughnessMap },
        { PBRComponent::NormalMap, UniformId::NormalMap },
        { PBRComponent::NormalMapScale, UniformId::NormalMapScale },
        { PBRComponent::OcclusionMap, UniformId::OcclusionMap },
        { PBRComponent::OcclusionMapStrength, UniformId::OcclusionMapStrength },
    };

    auto it = s_table.find(value);
    return (it == s_table.end()) ? UniformId::Undefined : it->second;
}

inline UniformId IProgram::uniformIdByBackgroundComponent(BackgroundComponent value)
{
    static const std::unordered_map<BackgroundComponent, UniformId> s_table {
        { BackgroundComponent::ColorMap, UniformId::BackgroundColorMap },
        { BackgroundComponent::BaseColor, UniformId::BackgroundBaseColor },
        { BackgroundComponent::Roughness, UniformId::BackgroundRoughness },
    };

    auto it = s_table.find(value);
    return (it == s_table.end()) ? UniformId::Undefined : it->second;
}

}
}
}

#endif // IGRAPHICSRENDERER_H

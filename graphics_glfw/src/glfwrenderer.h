#ifndef QTOPENGL_4_5_RENDERER_H
#define QTOPENGL_4_5_RENDERER_H

#include <tuple>
#include <deque>
#include <list>

#include <utils/noncopyble.h>

#include <core/graphicsrendererbase.h>

#include <graphics_glfw/forwarddecl.h>

#define CURRENT_CONTEXT_INFO \
    private: \
        std::shared_ptr<core::graphics::RendererBase> m_renderer; \
    public: \
        std::shared_ptr<core::graphics::RendererBase> renderer() const { return m_renderer; };

#define SAVE_CURRENT_CONTEXT \
    m_renderer = core::graphics::RendererBase::current(); \
    if (!m_renderer) LOG_CRITICAL << "No current context";

#define CHECK_CURRENT_CONTEXT \
    if (!core::graphics::RendererBase::areShared(m_renderer, core::graphics::RendererBase::current())) \
        LOG_CRITICAL << "Resource was created in anotrher context";

#define CHECK_THIS_CONTEXT \
    if (!core::graphics::RendererBase::areShared(shared_from_this(), core::graphics::RendererBase::current())) \
        LOG_CRITICAL << "This context is not current";

#define CHECK_RESOURCE_CONTEXT(resource) \
    if (!core::graphics::RendererBase::areShared(shared_from_this(), resource->renderer())) \
        LOG_CRITICAL << "Resource was created in anotrher context";


struct GLFWwindow;

namespace simplex
{
namespace graphics_glfw
{

class Conversions
{
public:
    static GLenum PrimitiveType2GL(utils::PrimitiveType);
    static GLenum VertexComponentType2GL(utils::VertexComponentType);
    static GLenum DrawElementsIndexType2GL(utils::DrawElementsIndexType);
    static GLenum PixelInternalFormat2GL(core::graphics::PixelInternalFormat);
    static core::graphics::PixelInternalFormat GL2PixelInternalFormat(GLenum);
    static GLenum PixelNumComponents2GL(uint32_t);
    static GLenum PixelComponentType2GL(utils::PixelComponentType);
    static GLenum FrameBufferAttachment2GL(core::graphics::FrameBufferAttachment);
    static GLenum TextureType2GL(core::graphics::TextureType);
    static GLenum TextureWrapMode2GL(core::graphics::TextureWrapMode);
    static GLenum TextureSwizzle2GL(core::graphics::TextureSwizzle);
    static GLbitfield BufferMapAccess2GL(core::graphics::IBuffer::MapAccess);
    static GLenum ImageDataAccess2GL(core::graphics::IImage::DataAccess);
    static uint16_t GL2VertexNumComponents(GLenum); // for shader vertex attribute
    static utils::VertexComponentType GL2VertexComponentType(GLenum); // for shader vertex attribute
    static GLenum UniformType2GL(core::graphics::UniformType);
    static core::graphics::UniformType GL2UniformType(GLenum);
    static GLenum FaceType2GL(core::graphics::FaceType);
    static GLenum ComparingFunc2GL(core::graphics::ComparingFunc);
    static GLenum StencilOperation2GL(core::graphics::StencilOperation);
    static GLenum BlendEquetion2GL(core::graphics::BlendEquation);
    static GLenum BlendFactor2GL(core::graphics::BlendFactor);
};

class BufferBase_4_5 : public std::enable_shared_from_this<BufferBase_4_5>
{
    NONCOPYBLE(BufferBase_4_5)
    CURRENT_CONTEXT_INFO
public:
    class MappedData_4_5 : public core::graphics::IBuffer::MappedData
    {
        CURRENT_CONTEXT_INFO
    public:
        MappedData_4_5(const std::weak_ptr<const BufferBase_4_5>&, uint8_t*);
        ~MappedData_4_5() override;
        const uint8_t* get() const override;
        uint8_t* get() override;
    private:
        std::weak_ptr<const BufferBase_4_5> m_mappedBuffer;
        uint8_t* m_data;
    };

    virtual ~BufferBase_4_5();

    GLuint id() const;

    size_t fullSize() const;
    void setFullSize(size_t size);

protected:
    BufferBase_4_5(uint64_t size, const void* data);

    std::unique_ptr<MappedData_4_5> mapData(core::graphics::IBuffer::MapAccess access, size_t offset = 0u, size_t size = 0u);
    std::unique_ptr<const MappedData_4_5> mapData(core::graphics::IBuffer::MapAccess access, size_t offset = 0u, size_t size = 0u) const;

private:
    GLuint m_id = 0;
    mutable bool m_isMapped = false;

    friend class MappedData_4_5;
};

class Buffer_4_5 : public core::graphics::IBuffer, public BufferBase_4_5
{
    NONCOPYBLE(Buffer_4_5)
    CURRENT_CONTEXT_INFO
public:
    Buffer_4_5(uint64_t size, const void* data);
    ~Buffer_4_5() override;

    size_t size() const override;
    void resize(size_t size) override;

    std::unique_ptr<const MappedData> map(MapAccess access, size_t offset = 0u, size_t size = 0u) const override;
    std::unique_ptr<MappedData> map(MapAccess access, size_t offset = 0u, size_t size = 0u) override;

    static std::shared_ptr<Buffer_4_5> create(size_t = 0u, const void* = nullptr);
};

class DynamicBuffer_4_5 : public core::graphics::IDynamicBuffer, public BufferBase_4_5
{
    NONCOPYBLE(DynamicBuffer_4_5)
    CURRENT_CONTEXT_INFO
public:
    DynamicBuffer_4_5(uint64_t size, const void* data);
    ~DynamicBuffer_4_5() override;

    size_t capacity() const override;
    void reserve(size_t) override;

    void pushBack(const void* data, size_t size);

    size_t size() const override;
    void resize(size_t size) override;

    std::unique_ptr<const MappedData> map(MapAccess access, size_t offset = 0u, size_t size = 0u) const override;
    std::unique_ptr<MappedData> map(MapAccess access, size_t offset = 0u, size_t size = 0u) override;

    static std::shared_ptr<DynamicBuffer_4_5> create(size_t = 0u, const void* = nullptr);

private:
    size_t m_size = 0;

    void expand(size_t);
};

class VertexArray_4_5 : public core::graphics::IVertexArray
{
    NONCOPYBLE(VertexArray_4_5)
    CURRENT_CONTEXT_INFO
public:
    VertexArray_4_5();
    ~VertexArray_4_5() override;

    GLuint id() const;

    uint32_t attachVertexBuffer(std::shared_ptr<core::graphics::IBuffer> buffer, size_t offset, uint32_t stride) override;
    void detachVertexBuffer(uint32_t bindingIndex) override;
    std::shared_ptr<const core::graphics::IBuffer> vertexBuffer(uint32_t bindingIndex) const override;
    size_t vertexBufferOffset(uint32_t bindingIndex) const override;
    uint32_t vertexBufferStride(uint32_t bindingIndex) const override;

    void declareVertexAttribute(utils::VertexAttribute,
                                uint32_t bindingIndex,
                                uint32_t numComponents,
                                utils::VertexComponentType type,
                                uint32_t relativeOffset) override;
    void undeclareVertexAttribute(utils::VertexAttribute) override;
    uint32_t vertexAttributeBindingIndex(utils::VertexAttribute) const override;
    uint32_t vertexAttributeNumComponents(utils::VertexAttribute) const override;
    utils::VertexComponentType vertexAttributeComponentType(utils::VertexAttribute) const override;
    uint32_t vertexAttributeRelativeOffset(utils::VertexAttribute) const override;

    void attachIndexBuffer(std::shared_ptr<core::graphics::IBuffer> buffer) override;
    void detachIndexBuffer() override;
    std::shared_ptr<const core::graphics::IBuffer> indexBuffer() const override;

    void addPrimitiveSet(std::shared_ptr<utils::PrimitiveSet>) override;
    void removePrimitiveSet(std::shared_ptr<utils::PrimitiveSet>) override;
    const std::unordered_set<std::shared_ptr<utils::PrimitiveSet>> &primitiveSets() const override;

    static std::shared_ptr<VertexArray_4_5> create(const std::shared_ptr<const utils::Mesh> & = nullptr, bool uniteVertexBuffers = true);

private:
    GLuint m_id = 0;

    std::vector<std::tuple<std::shared_ptr<Buffer_4_5>, size_t, uint32_t>> m_vertexBuffers;
    std::unordered_map<utils::VertexAttribute, std::tuple<uint32_t, uint32_t, utils::VertexComponentType, uint32_t>> m_vertexDeclarations;

    std::shared_ptr<Buffer_4_5> m_indexBuffer;

    std::unordered_set<std::shared_ptr<utils::PrimitiveSet>> m_primitiveSets;
};

class TextureBase_4_5 : public core::graphics::ITexture
{
    NONCOPYBLE(TextureBase_4_5)
    CURRENT_CONTEXT_INFO
public:
    TextureBase_4_5();
    ~TextureBase_4_5() override;

    GLuint id() const;
    GLenum GLinternalFormat() const;

    glm::uvec2 size() const override;
    core::graphics::PixelInternalFormat internalFormat() const override;
    bool hasAlpha() const override;
    bool hasDepth() const override;

    glm::uvec3 mipmapSize(uint32_t level = 0) const override;
    uint32_t numMipmapLevels() const override;
    uint32_t numFaces() const override;

    void subImage(uint32_t level,
                  const glm::uvec3 &offset,
                  const glm::uvec3 &size,
                  uint32_t numComponents,
                  utils::PixelComponentType type,
                  size_t bufSize,
                  void *data) const override;

    void generateMipmaps() override;
    void setBorderColor(const glm::vec4&) override;
    void setWrapMode(core::graphics::TextureWrapMode) override;
    void setFilterMode(core::graphics::TextureFilterMode) override;
    void setSwizzleMask(const core::graphics::TextureSwizzleMask&) override;

    core::graphics::PTexture copy() const override;

protected:
    GLuint m_id;
};

class Texture1D_4_5 : public TextureBase_4_5
{
    NONCOPYBLE(Texture1D_4_5)
    CURRENT_CONTEXT_INFO
public:
    Texture1D_4_5(uint32_t width, core::graphics::PixelInternalFormat, uint32_t numLevels);
    ~Texture1D_4_5() override;

    core::graphics::TextureType type() const override;

    void setSubImage(uint32_t level,
                     const glm::uvec3 &offset,
                     const glm::uvec3 &size,
                     uint32_t numComponents,
                     utils::PixelComponentType type,
                     const void *data) override;

    core::graphics::PTexture copyEmpty() const override;

    static std::shared_ptr<Texture1D_4_5> createEmpty(
        uint32_t width,
        core::graphics::PixelInternalFormat internalFormat,
        uint32_t numLevels);
    static std::shared_ptr<Texture1D_4_5> create(const std::shared_ptr<const utils::Image>&,
        core::graphics::PixelInternalFormat = core::graphics::PixelInternalFormat::Undefined,
        uint32_t numLevels = 0,
        bool genMipmaps = true);

};

class Texture2D_4_5 : public TextureBase_4_5
{
    NONCOPYBLE(Texture2D_4_5)
    CURRENT_CONTEXT_INFO
public:
    Texture2D_4_5(uint32_t width, uint32_t height, core::graphics::PixelInternalFormat, uint32_t numLevels);
    ~Texture2D_4_5() override;

    core::graphics::TextureType type() const override;

    void setSubImage(uint32_t level,
                     const glm::uvec3 &offset,
                     const glm::uvec3 &size,
                     uint32_t numComponents,
                     utils::PixelComponentType type,
                     const void *data) override;

    core::graphics::PTexture copyEmpty() const override;

    static std::shared_ptr<Texture2D_4_5> createEmpty(uint32_t width,
        uint32_t height,
        core::graphics::PixelInternalFormat,
        uint32_t numLevels = 1);
    static std::shared_ptr<Texture2D_4_5> create(const std::shared_ptr<const utils::Image>&,
        core::graphics::PixelInternalFormat = core::graphics::PixelInternalFormat::Undefined,
        uint32_t numLevels = 0,
        bool genMipmaps = true);

};

class Texture3D_4_5 : public TextureBase_4_5
{
    NONCOPYBLE(Texture3D_4_5)
    CURRENT_CONTEXT_INFO
public:
    Texture3D_4_5(uint32_t width, uint32_t height, uint32_t depth, core::graphics::PixelInternalFormat, uint32_t numLevels);
    ~Texture3D_4_5() override;

    core::graphics::TextureType type() const override;

    void setSubImage(uint32_t level,
                     const glm::uvec3 &offset,
                     const glm::uvec3 &size,
                     uint32_t numComponents,
                     utils::PixelComponentType type,
                     const void *data) override;

    core::graphics::PTexture copyEmpty() const override;

    static std::shared_ptr<Texture3D_4_5> createEmpty(uint32_t width,
        uint32_t height,
        uint32_t depth,
        core::graphics::PixelInternalFormat,
        uint32_t numLevels = 1);
    static std::shared_ptr<Texture3D_4_5> create(const std::vector<std::shared_ptr<const utils::Image>>&,
        core::graphics::PixelInternalFormat = core::graphics::PixelInternalFormat::Undefined,
        uint32_t numLevels = 0,
        bool genMipmaps = true);

};

class TextureCube_4_5 : public TextureBase_4_5
{
    NONCOPYBLE(TextureCube_4_5)
    CURRENT_CONTEXT_INFO
public:
    TextureCube_4_5(uint32_t width, uint32_t height, core::graphics::PixelInternalFormat, uint32_t numLevels);
    ~TextureCube_4_5() override;

    core::graphics::TextureType type() const override;

    uint32_t numFaces() const override;

    void setSubImage(uint32_t level,
                     const glm::uvec3 &offset, // offset.z is face to update
                     const glm::uvec3 &size, // size.z is number of faces to update
                     uint32_t numComponents,
                     utils::PixelComponentType type,
                     const void *data) override;

    core::graphics::PTexture copyEmpty() const override;

    static std::shared_ptr<TextureCube_4_5> createEmpty(uint32_t width,
        uint32_t height,
        core::graphics::PixelInternalFormat,
        uint32_t numLevels = 1);
    static std::shared_ptr<TextureCube_4_5> create(const std::vector<std::shared_ptr<const utils::Image>>&,
        core::graphics::PixelInternalFormat = core::graphics::PixelInternalFormat::Undefined,
        uint32_t numLevels = 0,
        bool genMipmaps = true);

};

class Texture1DArray_4_5 : public TextureBase_4_5
{
    NONCOPYBLE(Texture1DArray_4_5)
    CURRENT_CONTEXT_INFO
public:
    Texture1DArray_4_5(uint32_t width, uint32_t numLayers, core::graphics::PixelInternalFormat, uint32_t numLevels);
    ~Texture1DArray_4_5() override;

    core::graphics::TextureType type() const override;

    void setSubImage(uint32_t level,
                     const glm::uvec3 &offset, // offset.y is layer to update
                     const glm::uvec3 &size, // size.y is number of layers to update
                     uint32_t numComponents,
                     utils::PixelComponentType type,
                     const void *data) override;

    core::graphics::PTexture copyEmpty() const override;

    static std::shared_ptr<Texture1DArray_4_5> createEmpty(uint32_t width,
        uint32_t numLayers,
        core::graphics::PixelInternalFormat,
        uint32_t numLevels = 1);
    static std::shared_ptr<Texture1DArray_4_5> create(const std::vector<std::shared_ptr<const utils::Image>>&,
        core::graphics::PixelInternalFormat = core::graphics::PixelInternalFormat::Undefined,
        uint32_t numLevels = 0,
        bool genMipmaps = true);

};

class Texture2DArray_4_5 : public TextureBase_4_5
{
    NONCOPYBLE(Texture2DArray_4_5)
    CURRENT_CONTEXT_INFO
public:
    Texture2DArray_4_5(uint32_t width, uint32_t height, uint32_t numLayers, core::graphics::PixelInternalFormat, uint32_t numLevels);
    ~Texture2DArray_4_5() override;

    core::graphics::TextureType type() const override;

    void setSubImage(uint32_t level,
                     const glm::uvec3 &offset, // offset.z is layer to update
                     const glm::uvec3 &size, // size.z is number of layers to update
                     uint32_t numComponents,
                     utils::PixelComponentType type,
                     const void *data) override;

    core::graphics::PTexture copyEmpty() const override;

    static std::shared_ptr<Texture2DArray_4_5> createEmpty(uint32_t width,
        uint32_t height,
        uint32_t numLayers,
        core::graphics::PixelInternalFormat,
        uint32_t numLevels = 1);
    static std::shared_ptr<Texture2DArray_4_5> create(const std::vector<std::shared_ptr<const utils::Image>>&,
        core::graphics::PixelInternalFormat = core::graphics::PixelInternalFormat::Undefined,
        uint32_t numLevels = 0,
        bool genMipmaps = true);
};

class TextureCubeArray_4_5 : public TextureBase_4_5
{
    NONCOPYBLE(TextureCubeArray_4_5)
    CURRENT_CONTEXT_INFO
public:
    TextureCubeArray_4_5(uint32_t width, uint32_t height, uint32_t numLayers, core::graphics::PixelInternalFormat, uint32_t numLevels);
    ~TextureCubeArray_4_5() override;

    core::graphics::TextureType type() const override;

    uint32_t numFaces() const override;

    void setSubImage(uint32_t level,
                     const glm::uvec3 &offset, // offset.z is first layer-face to update
                     const glm::uvec3 &size, // size.z is number of layer-faces to update
                     uint32_t numComponents,
                     utils::PixelComponentType type,
                     const void *data) override;

    core::graphics::PTexture copyEmpty() const override;

    static std::shared_ptr<TextureCubeArray_4_5> createEmpty(uint32_t width,
        uint32_t height,
        uint32_t numLayers,
        core::graphics::PixelInternalFormat,
        uint32_t numLevels = 1);
    static std::shared_ptr<TextureCubeArray_4_5> create(const std::vector<std::vector<std::shared_ptr<const utils::Image>>>&,
        core::graphics::PixelInternalFormat = core::graphics::PixelInternalFormat::Undefined,
        uint32_t numLevels = 0,
        bool genMipmaps = true);

};

class TextureRect_4_5 : public TextureBase_4_5
{
    NONCOPYBLE(TextureRect_4_5)
    CURRENT_CONTEXT_INFO
public:
    TextureRect_4_5(uint32_t width, uint32_t height, core::graphics::PixelInternalFormat);
    ~TextureRect_4_5() override;

    core::graphics::TextureType type() const override;

    void setSubImage(uint32_t level,
                     const glm::uvec3 &offset,
                     const glm::uvec3 &size,
                     uint32_t numComponents,
                     utils::PixelComponentType type,
                     const void *data) override;

    core::graphics::PTexture copyEmpty() const override;

    static std::shared_ptr<TextureRect_4_5> createEmpty(uint32_t width,
            uint32_t height,
            core::graphics::PixelInternalFormat);
    static std::shared_ptr<TextureRect_4_5> create(const std::shared_ptr<const utils::Image>&,
        core::graphics::PixelInternalFormat = core::graphics::PixelInternalFormat::Undefined);
};

class TextureHandle_4_5 : public core::graphics::ITextureHandle
{
    NONCOPYBLE(TextureHandle_4_5)
    CURRENT_CONTEXT_INFO
public:
    TextureHandle_4_5(const core::graphics::PConstTexture&);
    ~TextureHandle_4_5() override;

    core::graphics::TextureHandle handle() const override;

    core::graphics::PConstTexture texture() const override;

    void makeResident() override;
    void doneResident() override;

    static std::shared_ptr<TextureHandle_4_5> create(const core::graphics::PConstTexture&);

protected:
    GLuint64 m_id = 0;
    core::graphics::PConstTexture m_texture;
};

class Image_4_5 : public core::graphics::IImage
{
    NONCOPYBLE(Image_4_5)
    CURRENT_CONTEXT_INFO
public:
    Image_4_5(DataAccess, const core::graphics::PConstTexture&, uint32_t);
    ~Image_4_5() override;

    DataAccess access() const override;
    void setAccess(DataAccess) override;

    uint32_t mipmapLevel() const override;
    core::graphics::PixelInternalFormat format() const override;
    core::graphics::PConstTexture texture() const override;
    void setTexture(const core::graphics::PConstTexture&, uint32_t) override;

    static const core::graphics::SupportedImageFormats& supportedImageFormats();
    static std::shared_ptr<Image_4_5> create(core::graphics::IImage::DataAccess,
        const core::graphics::PConstTexture&,
        uint32_t level = 0u);

protected:
    core::graphics::PConstTexture m_texture;
    uint32_t m_level;
    core::graphics::PixelInternalFormat m_format;
    DataAccess m_access;
};

class RenderBuffer_4_5 : public core::graphics::IRenderBuffer
{
    NONCOPYBLE(RenderBuffer_4_5)
    CURRENT_CONTEXT_INFO
public:
    RenderBuffer_4_5(uint32_t width, uint32_t height, core::graphics::PixelInternalFormat);
    ~RenderBuffer_4_5() override;

    GLuint id() const;

    glm::uvec2 size() const override;
    core::graphics::PixelInternalFormat internalFormat() const override;
    bool hasAlpha() const override;
    bool hasDepth() const override;

    static std::shared_ptr<RenderBuffer_4_5> create(uint32_t width, uint32_t height, core::graphics::PixelInternalFormat);

protected:
    GLuint m_id = 0;
};

class FrameBufferBase_4_5 : public core::graphics::IFrameBuffer
{
    NONCOPYBLE(FrameBufferBase_4_5)
    CURRENT_CONTEXT_INFO
public:
    FrameBufferBase_4_5(GLuint id);
    ~FrameBufferBase_4_5() override;

    GLuint id() const;
    void clear();

    bool isComplete() const override;

    const Attachments &attachments() const override;
    bool attachment(core::graphics::FrameBufferAttachment, AttachmentInfo&) const override;

    const core::graphics::FrameBufferClearColor &clearColor(uint32_t) const override;
    void setClearColor(uint32_t, const glm::vec4&) override;
    void setClearColor(uint32_t, const glm::i32vec4&) override;
    void setClearColor(uint32_t, const glm::u32vec4&) override;

    float clearDepth() const override;
    int32_t clearStencil() const override;
    void setClearDepthStencil(float, uint8_t) override;

    const std::unordered_set<core::graphics::FrameBufferAttachment> &clearMask() const override;
    void setClearMask(const std::unordered_set<core::graphics::FrameBufferAttachment>&) override;

    void setDrawBuffers(const std::vector<core::graphics::FrameBufferAttachment>&) override;

    bool faceCulling() const override;
    core::graphics::FaceType cullFaceType() const override;
    void setFaceCulling(bool, core::graphics::FaceType = core::graphics::FaceType::Back) override;

    bool colorMask(uint32_t) const override;
    void setColorMask(uint32_t, bool) override;
    void setColorMasks(bool) override;

    bool depthTest() const override;
    core::graphics::ComparingFunc depthFunc() const override;
    void setDepthTest(bool, core::graphics::ComparingFunc func = core::graphics::ComparingFunc::Less) override;
    bool depthMask() const override;
    void setDepthMask(bool) override;

    bool stencilTest() const override;
    void setStencilTest(bool) override;
    core::graphics::ComparingFunc stencilComparingFunc(core::graphics::FaceType) const override;
    uint8_t stencilReferenceValue(core::graphics::FaceType) const override;
    virtual uint8_t stencilMaskValue(core::graphics::FaceType) const override;
    void setStencilFunc(core::graphics::FaceType, core::graphics::ComparingFunc, uint8_t ref, uint8_t mask) override;
    const core::graphics::StencilOperations &stencilOperations(core::graphics::FaceType) const override;
    void setStencilOperations(core::graphics::FaceType, const core::graphics::StencilOperations&) override;

    bool blending() const override;
    void setBlending(bool) override;
    core::graphics::BlendEquation blendColorEquation(uint32_t) override;
    core::graphics::BlendEquation blendAlphaEquation(uint32_t) override;
    void setBlendEquation(uint32_t, core::graphics::BlendEquation, core::graphics::BlendEquation) override;
    core::graphics::BlendFactor blendColorSourceFactor(uint32_t) override;
    core::graphics::BlendFactor blendAlphaSourceFactor(uint32_t) override;
    core::graphics::BlendFactor blendColorDestinationFactor(uint32_t) override;
    core::graphics::BlendFactor blendAlphaDestinationFactor(uint32_t) override;
    void setBlendFactor(uint32_t,
                        core::graphics::BlendFactor,
                        core::graphics::BlendFactor,
                        core::graphics::BlendFactor,
                        core::graphics::BlendFactor) override;
    glm::vec3 blendConstantColor() const override;
    void setBlendConstantColor(const glm::vec3&) override;
    float blendConstantAlpha() const override;
    void setBlendConstantAlpha(float) override;

protected:
    GLuint m_id;

    Attachments m_attachments;

    std::array<core::graphics::FrameBufferClearColor, core::graphics::FrameBufferColorAttachmentsCount()> m_clearColor;
    float m_clearDepth;
    int32_t m_clearStencil;
    std::unordered_set<core::graphics::FrameBufferAttachment> m_clearMask;

    core::graphics::FaceType m_cullFaceType;
    bool m_faceCulling;

    std::array<bool, core::graphics::FrameBufferColorAttachmentsCount()> m_colorMasks;

    core::graphics::ComparingFunc m_depthFunc;
    bool m_depthTest;
    bool m_depthMask;

    core::graphics::ComparingFunc m_stencilComparingFuncFrontFace, m_stencilComparingFuncBackFace;
    uint8_t m_stencilRefFrontFace, m_stencilRefBackFace;
    uint8_t m_stencilMaskFrontFace, m_stencilMaskBackFace;
    core::graphics::StencilOperations m_stencilOperationsFrontFace, m_stencilOperationsBackFace;
    bool m_stencilTest;

    std::array<core::graphics::BlendEquation, core::graphics::FrameBufferColorAttachmentsCount()> m_blendColorEquation, m_blendAlphaEquation;
    std::array<core::graphics::BlendFactor, core::graphics::FrameBufferColorAttachmentsCount()> m_blendColorSourceFactor, m_blendAlphaSourceFactor;
    std::array<core::graphics::BlendFactor, core::graphics::FrameBufferColorAttachmentsCount()> m_blendColorDestFactor, m_blendAlphaDestFactor;
    glm::vec3 m_blendConstColor;
    float m_blendConstAlpha;
    bool m_blending;
};

class FrameBuffer_4_5 : public FrameBufferBase_4_5
{
    NONCOPYBLE(FrameBuffer_4_5)
    CURRENT_CONTEXT_INFO
public:
    FrameBuffer_4_5();
    ~FrameBuffer_4_5() override;

    void attach(core::graphics::FrameBufferAttachment,
                std::shared_ptr<const core::graphics::ISurface>,
                uint32_t level = 0u) override;
    void attachLayer(core::graphics::FrameBufferAttachment,
                     std::shared_ptr<const core::graphics::ITexture>,
                     uint32_t level = 0u,
                     uint32_t layer = 0u) override;
    void detach(core::graphics::FrameBufferAttachment) override;

    static std::shared_ptr<FrameBuffer_4_5> create();
};

class DefaultFrameBuffer_4_5 : public FrameBufferBase_4_5
{
    NONCOPYBLE(DefaultFrameBuffer_4_5)
    CURRENT_CONTEXT_INFO
public:
    DefaultFrameBuffer_4_5(GLuint);
    ~DefaultFrameBuffer_4_5() override;

    void attach(core::graphics::FrameBufferAttachment, std::shared_ptr<const core::graphics::ISurface>, uint32_t = 0u) override;
    void attachLayer(core::graphics::FrameBufferAttachment,
                     std::shared_ptr<const core::graphics::ITexture>,
                     uint32_t = 0u,
                     uint32_t = 0u) override;
    void detach(core::graphics::FrameBufferAttachment) override;

    void setClearColor(uint32_t, const glm::vec4&) override;
    void setClearColor(uint32_t, const glm::i32vec4&) override;
    void setClearColor(uint32_t, const glm::u32vec4&) override;
    void setClearDepthStencil(float, uint8_t) override;
    void setClearMask(const std::unordered_set<core::graphics::FrameBufferAttachment>&) override;

    static std::shared_ptr<DefaultFrameBuffer_4_5> create(GLuint);
};

class ProgramBase_4_5 : public virtual core::graphics::IProgram
{
    NONCOPYBLE(ProgramBase_4_5)
    CURRENT_CONTEXT_INFO
public:
    ProgramBase_4_5();
    ~ProgramBase_4_5() override;

    bool compileAndLink(const std::unordered_map<GLenum, std::reference_wrapper<const std::string>>&);

    GLuint id() const;
    virtual bool preBuild(std::string&);
    virtual bool postBuild(std::string&);

    int32_t uniformLocationByName(const std::string&) const override;

    const std::vector<core::graphics::UniformInfo> &uniformsInfo() const override;
    const std::vector<core::graphics::SSBOInfo> &SSBOsInfo() const override;

    std::string uniformNameByIndex(uint16_t) const override;
    std::string SSBOVariableNameByIndex(uint16_t) const override;
    std::string SSBONameByIndex(uint16_t) const override;

protected:
    GLuint m_id;

    std::vector<core::graphics::UniformInfo> m_uniformsInfo;
    std::vector<core::graphics::SSBOInfo> m_SSBOsInfo;

    GLint m_uniformNameMaxLength;
    GLint m_SSBONameMaxLength;
    GLint m_bufferVariableNameMaxLength;
};

class RenderProgram_4_5 : public core::graphics::IRenderProgram, public ProgramBase_4_5
{
    NONCOPYBLE(RenderProgram_4_5)
    CURRENT_CONTEXT_INFO
public:
    RenderProgram_4_5();
    ~RenderProgram_4_5() override;

    bool postBuild(std::string&) override;

    int32_t attributeLocationByName(const std::string&) const override;
    const std::vector<core::graphics::AttributeInfo> &attributesInfo() const override;
    std::string attributeNameByIndex(uint16_t) const override;

    static std::shared_ptr<RenderProgram_4_5> create(const std::shared_ptr<utils::Shader>& vertexShader,
        const std::shared_ptr<utils::Shader>& fragmentShader);
    static std::shared_ptr<RenderProgram_4_5> create(const std::shared_ptr<utils::Shader>& vertexShader,
        const std::shared_ptr<utils::Shader>& geometryShader,
        const std::shared_ptr<utils::Shader>& fragmentShader);

protected:
    std::vector<core::graphics::AttributeInfo> m_attributesInfo;

    GLint m_attributeNameMaxLength;
};

class ComputeProgram_4_5 : public core::graphics::IComputeProgram, public ProgramBase_4_5
{
    NONCOPYBLE(ComputeProgram_4_5)
    CURRENT_CONTEXT_INFO
public:
    ComputeProgram_4_5();
    ~ComputeProgram_4_5() override;

    bool postBuild(std::string&) override;

    glm::uvec3 workGroupSize() const override;

    static std::shared_ptr<ComputeProgram_4_5> create(const std::shared_ptr<utils::Shader>& computeShader);

protected:
    glm::uvec3 m_workGroupSize;

};

class GLFWRenderer : public core::graphics::RendererBase
{
    NONCOPYBLE(GLFWRenderer)
public:
    GLFWRenderer(const std::string&, const std::weak_ptr<GLFWWidget>&);
    ~GLFWRenderer() override;

    std::shared_ptr<core::graphics::IGraphicsWidget> widget();
    std::shared_ptr<const core::graphics::IGraphicsWidget> widget() const;

    void blitFrameBuffer(std::shared_ptr<const core::graphics::IFrameBuffer> src,
                         std::shared_ptr<core::graphics::IFrameBuffer> dst,
                         const glm::uvec4 &srcViewport,
                         const glm::uvec4 &dstViewport,
                         bool colorMsk, bool depthMask, bool stencilMask,
                         bool linearFilter = false) override;

    void copyBufferSubData(const std::shared_ptr<core::graphics::IBuffer>& dst,
        const std::shared_ptr<const core::graphics::IBuffer>& src,
        size_t dstOffset,
        size_t srcOffset,
        size_t size) const override;

    bool registerVertexAttribute(const std::string&, utils::VertexAttribute) override;
    bool unregisterVertexAttribute(const std::string&) override;
    utils::VertexAttribute vertexAttributeByName(const std::string&) const override;

    bool registerUniformId(const std::string&, uint16_t) override;
    bool unregisterUniformId(const std::string&) override;
    uint16_t uniformIdByName(const std::string&) const override;

    bool registerSSBOId(const std::string&, uint16_t) override;
    bool unregisterSSBOId(const std::string&) override;
    uint16_t SSBOIdByName(const std::string&) const override;

    std::shared_ptr<core::graphics::IBuffer> createBuffer(size_t = 0u,
                                                          const void* = nullptr) const override;
    std::shared_ptr<core::graphics::IDynamicBuffer> createDynamicBuffer(size_t size = 0u,
                                                                              const void* data = nullptr) const override;
    std::shared_ptr<core::graphics::IVertexArray> createVertexArray(const std::shared_ptr<const utils::Mesh>& = nullptr,
                                                                    bool uniteVertexBuffers = true) const override;
    std::shared_ptr<core::graphics::ITexture> createTexture1DEmpty(uint32_t width,
                                                                   core::graphics::PixelInternalFormat,
                                                                   uint32_t numLevels = 1) const override;
    std::shared_ptr<core::graphics::ITexture> createTexture1D(const std::shared_ptr<const utils::Image>&,
                                                              core::graphics::PixelInternalFormat = core::graphics::PixelInternalFormat::Undefined,
                                                              uint32_t numLevels = 0,
                                                              bool genMipmaps = true) const override;
    std::shared_ptr<core::graphics::ITexture> createTexture2DEmpty(uint32_t width,
                                                                   uint32_t height,
                                                                   core::graphics::PixelInternalFormat,
                                                                   uint32_t numLevels = 1) const override;
    std::shared_ptr<core::graphics::ITexture> createTexture2D(const std::shared_ptr<const utils::Image>&,
                                                              core::graphics::PixelInternalFormat = core::graphics::PixelInternalFormat::Undefined,
                                                              uint32_t numLevels = 0,
                                                              bool genMipmaps = true) const override;
    std::shared_ptr<core::graphics::ITexture> createTexture3DEmpty(uint32_t width,
                                                                   uint32_t height,
                                                                   uint32_t depth,
                                                                   core::graphics::PixelInternalFormat,
                                                                   uint32_t numLevels = 1) const override;
    std::shared_ptr<core::graphics::ITexture> createTexture3D(const std::vector<std::shared_ptr<const utils::Image>>&,
                                                              core::graphics::PixelInternalFormat = core::graphics::PixelInternalFormat::Undefined,
                                                              uint32_t numLevels = 0,
                                                              bool genMipmaps = true) const override;
    std::shared_ptr<core::graphics::ITexture> createTextureCubeEmpty(uint32_t width,
                                                                     uint32_t height,
                                                                     core::graphics::PixelInternalFormat,
                                                                     uint32_t numLevels = 1) const override;
    std::shared_ptr<core::graphics::ITexture> createTextureCube(const std::vector<std::shared_ptr<const utils::Image>>&,
                                                                core::graphics::PixelInternalFormat = core::graphics::PixelInternalFormat::Undefined,
                                                                uint32_t numLevels = 0,
                                                                bool genMipmaps = true) const override;
    std::shared_ptr<core::graphics::ITexture> createTexture1DArrayEmpty(uint32_t width,
                                                                        uint32_t numLayers,
                                                                        core::graphics::PixelInternalFormat,
                                                                        uint32_t numLevels = 1) const override;
    std::shared_ptr<core::graphics::ITexture> createTexture1DArray(const std::vector<std::shared_ptr<const utils::Image>>&,
                                                                   core::graphics::PixelInternalFormat = core::graphics::PixelInternalFormat::Undefined,
                                                                   uint32_t numLevels = 0,
                                                                   bool genMipmaps = true) const override;
    std::shared_ptr<core::graphics::ITexture> createTexture2DArrayEmpty(uint32_t width,
                                                                        uint32_t height,
                                                                        uint32_t numLayers,
                                                                        core::graphics::PixelInternalFormat,
                                                                        uint32_t numLevels = 1) const override;
    std::shared_ptr<core::graphics::ITexture> createTexture2DArray(const std::vector<std::shared_ptr<const utils::Image>>&,
                                                                   core::graphics::PixelInternalFormat = core::graphics::PixelInternalFormat::Undefined,
                                                                   uint32_t numLevels = 0,
                                                                   bool genMipmaps = true) const override;
    std::shared_ptr<core::graphics::ITexture> createTextureCubeArrayEmpty(uint32_t width,
                                                                          uint32_t height,
                                                                          uint32_t numLayers,
                                                                          core::graphics::PixelInternalFormat,
                                                                          uint32_t numLevels = 1) const override;
    std::shared_ptr<core::graphics::ITexture> createTextureCubeArray(const std::vector<std::vector<std::shared_ptr<const utils::Image>>>&,
                                                                     core::graphics::PixelInternalFormat = core::graphics::PixelInternalFormat::Undefined,
                                                                     uint32_t numLevels = 0,
                                                                     bool genMipmaps = true) const override;
    std::shared_ptr<core::graphics::ITexture> createTextureRectEmpty(uint32_t width,
                                                                   uint32_t height,
                                                                   core::graphics::PixelInternalFormat) const override;
    std::shared_ptr<core::graphics::ITexture> createTextureRect(const std::shared_ptr<const utils::Image>&,
                                                                core::graphics::PixelInternalFormat = core::graphics::PixelInternalFormat::Undefined) const override;
    std::shared_ptr<core::graphics::ITextureHandle> createTextureHandle(const core::graphics::PConstTexture&) const override;
    std::shared_ptr<core::graphics::IImage> createImage(core::graphics::IImage::DataAccess,
                                                        const core::graphics::PConstTexture&,
                                                        uint32_t level = 0u) const override;
    std::shared_ptr<core::graphics::IRenderBuffer> createRenderBuffer(uint32_t width,
                                                                      uint32_t height,
                                                                      core::graphics::PixelInternalFormat) const override;
    std::shared_ptr<core::graphics::IFrameBuffer> createFrameBuffer() const override;
    std::shared_ptr<core::graphics::IRenderProgram> createRenderProgram(const std::shared_ptr<utils::Shader> &vertexShader,
                                                                        const std::shared_ptr<utils::Shader> &fragmentShader) const override;
    std::shared_ptr<core::graphics::IRenderProgram> createRenderProgram(const std::shared_ptr<utils::Shader> &vertexShader,
                                                                        const std::shared_ptr<utils::Shader> &geometryShader,
                                                                        const std::shared_ptr<utils::Shader> &fragmentShader) const override;
    std::shared_ptr<core::graphics::IComputeProgram> createComputeProgram(const std::shared_ptr<utils::Shader> &computeShader) const override;

    const core::graphics::SupportedImageFormats &supportedImageFormats() const override;

    const glm::uvec2 &screenSize() const override;
    void resize(const glm::uvec2&) override;

    void clearRenderData() override;
    void addRenderData(const std::shared_ptr<core::graphics::IRenderProgram>&,
                       const std::shared_ptr<const core::Drawable>&,
                       const glm::mat4x4& = glm::mat4x4(1.f)) override;
    void render(const std::shared_ptr<core::graphics::IFrameBuffer>&,
                const glm::uvec4 &viewport,
                const glm::mat4x4 &viewMatrix,
                const glm::mat4x4 &projectionMatrix,
                const core::PConstStateSet&) override;
    void compute(const std::shared_ptr<core::graphics::IComputeProgram>&,
                 const glm::uvec3&,
                 const core::PConstStateSet&) override;

protected:
    bool doMakeCurrent() override;
    bool doDoneCurrent() override;

private:
    void setupVertexAttributes(const std::shared_ptr<RenderProgram_4_5>&,
                               const std::shared_ptr<const VertexArray_4_5>&);
    void setupUniform(GLuint rpId, GLint loc, int32_t&, int32_t&, const core::PConstAbstractUniform&);
    void setupUniforms(const std::shared_ptr<ProgramBase_4_5>&,
                       const std::list<core::PConstStateSet>&);
    void setupSSBOs(const std::shared_ptr<ProgramBase_4_5>&,
                    const std::list<core::PConstStateSet>&);

    void bindTexture(int32_t, const core::graphics::PConstTexture&);
    void bindImage(int32_t, const core::graphics::PConstImage&);
    void bindBuffer(GLenum target, GLuint bindingPoint, const core::graphics::PConstBuffer&);
    void bindSSBO(uint32_t, const core::graphics::PConstBuffer&);
    void bindAtomicCounterBuffer(GLuint bindingPoint, const core::graphics::PConstBuffer&);

    std::weak_ptr<GLFWWidget> m_widget;

    std::unordered_map<std::string, utils::VertexAttribute> m_attributeIds;
    std::unordered_map<std::string, uint16_t> m_uniformIds;
    std::unordered_map<std::string, uint16_t> m_SSBOIds;

    std::deque<std::tuple<glm::mat4x4, std::shared_ptr<RenderProgram_4_5>, std::shared_ptr<const core::Drawable>>> m_renderData;
    glm::uvec2 m_screenSize;
};

}
}

#endif // QTOPENGL_4_5_RENDERER_H

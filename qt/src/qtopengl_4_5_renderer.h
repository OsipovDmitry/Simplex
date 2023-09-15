#ifndef QTOPENGL_1_0_RENDERER_H
#define QTOPENGL_1_0_RENDERER_H

#include <tuple>
#include <list>

#include <QOpenGLFunctions_4_5_Core>

#include <utils/noncopyble.h>

#include <core/igraphicsrenderer.h>

namespace simplex
{
namespace qt
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
    static core::graphics::PixelInternalFormat PixelNumComponentsAndPixelComponentTypeToPixelInternalFormat(uint32_t, utils::PixelComponentType);
    static GLenum FrameBufferAttachment2GL(core::graphics::FrameBufferAttachment);
    static GLenum TextureWrapMode2GL(core::graphics::TextureWrapMode);
    static GLbitfield BufferMapAccess2GL(core::graphics::IBuffer::MapAccess);
    static GLenum ImageDataAccess2GL(core::graphics::IImage::DataAccess);
    static uint16_t GL2VertexNumComponents(GLenum); // for shader vertex attribute
    static utils::VertexComponentType GL2VertexComponentType(GLenum); // for shader vertex attribute
    static GLenum UniformType2GL(core::graphics::UniformType);
    static core::graphics::UniformType GL2UniformType(GLenum);
    static GLenum faceType2GL(core::graphics::FaceType);
    static GLenum depthFunc2GL(core::graphics::DepthFunc);
};

class Buffer_4_5 : public core::graphics::IBuffer, public std::enable_shared_from_this<Buffer_4_5>
{
    NONCOPYBLE(Buffer_4_5)
public:
    class MappedData_4_5 : public core::graphics::IBuffer::MappedData
    {
    public:
        MappedData_4_5(std::weak_ptr<const Buffer_4_5>, uint8_t*);
        ~MappedData_4_5() override;
        const uint8_t *get() const override;
        uint8_t *get() override;
    private:
        std::weak_ptr<const Buffer_4_5> m_mappedBuffer;
        uint8_t *m_data;
    };

    Buffer_4_5(uint64_t size, const void *data);
    ~Buffer_4_5() override;

    GLuint id() const;

    size_t size() const override;
    void resize(size_t size, const void *data) override;

    std::unique_ptr<const MappedData> map(MapAccess access, size_t offset = 0u, size_t size = 0u) const override;
    std::unique_ptr<MappedData> map(MapAccess access, size_t offset = 0u, size_t size = 0u) override;

private:
    GLuint m_id = 0;
    mutable bool m_isMapped = false;


    friend class MappedData_4_5;
};

class BufferRange_4_5 : public core::graphics::IBufferRange
{
public:
    BufferRange_4_5(std::shared_ptr<core::graphics::IBuffer>, size_t, size_t);
    ~BufferRange_4_5() override;

    std::shared_ptr<const core::graphics::IBuffer> buffer() const override;
    std::shared_ptr<core::graphics::IBuffer> buffer() override;

    size_t offset() const override;
    size_t size() const override;

private:
    std::shared_ptr<core::graphics::IBuffer> m_buffer;
    size_t m_offset;
    size_t m_size;
};

class VertexArray_4_5 : public core::graphics::IVertexArray
{
    NONCOPYBLE(VertexArray_4_5)
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
    utils::VertexAttributesSet vertexAttributesSet() const override;
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
public:
    TextureBase_4_5();
    ~TextureBase_4_5() override;

    GLuint id() const;

    glm::uvec2 size() const override;
    core::graphics::PixelInternalFormat internalFormat() const override;
    bool hasAlpha() const override;

    glm::uvec3 mipmapSize(uint32_t level = 0) const override;
    uint32_t numMipmapLevels() const override;

    void generateMipmaps() override;
    void setBorderColor(const glm::vec4&) override;
    void setWrapMode(core::graphics::TextureWrapMode) override;
    void setFiltering(core::graphics::TextureFiltering) override;

    GLenum GLinternalFormat() const;

protected:
    GLuint m_id = 0;
};

class Texture2D_4_5 : public TextureBase_4_5
{
    NONCOPYBLE(Texture2D_4_5)
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

};

class TextureRect_4_5 : public TextureBase_4_5
{
    NONCOPYBLE(TextureRect_4_5)
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
};

class Image_4_5 : public core::graphics::IImage
{
public:
    Image_4_5(std::shared_ptr<core::graphics::ITexture>, DataAccess, uint32_t);
    ~Image_4_5() override;

    std::shared_ptr<const core::graphics::ITexture> texture() const override;
    std::shared_ptr<core::graphics::ITexture> texture() override;

    DataAccess access() const override;

    uint32_t mipmapLevel() const override;

protected:
    std::shared_ptr<core::graphics::ITexture> m_texture;
    uint32_t m_level;
    DataAccess m_access;
};

class RenderBuffer_4_5 : public core::graphics::IRenderBuffer
{
    NONCOPYBLE(RenderBuffer_4_5)
public:
    RenderBuffer_4_5(uint32_t width, uint32_t height, core::graphics::PixelInternalFormat);
    ~RenderBuffer_4_5() override;

    GLuint id() const;

    glm::uvec2 size() const override;
    core::graphics::PixelInternalFormat internalFormat() const override;
    bool hasAlpha() const override;

protected:
    GLuint m_id = 0;
};

class FrameBufferBase_4_5 : public core::graphics::IFrameBuffer
{
    NONCOPYBLE(FrameBufferBase_4_5)
public:
    FrameBufferBase_4_5(GLuint id);
    ~FrameBufferBase_4_5() override;

    GLuint id() const;

    bool isComplete() const override;

    const Attachments &attachments() const override;
    bool attachment(core::graphics::FrameBufferAttachment, AttachmentInfo&) const override;

    const core::graphics::FrameBufferClearColor &clearColor(uint32_t) const override;
    void setClearColor(uint32_t, const glm::vec4&) override;
    void setClearColor(uint32_t, const glm::i32vec4&) override;
    void setClearColor(uint32_t, const glm::u32vec4&) override;

    float clearDepth() const override;
    int32_t clearStencil() const override;
    void setClearDepthStencil(float, int32_t) override;

    const std::unordered_set<core::graphics::FrameBufferAttachment> &clearMask() const override;
    void setClearMask(const std::unordered_set<core::graphics::FrameBufferAttachment>&) override;

    void setDrawBuffers(const std::vector<core::graphics::FrameBufferAttachment>&) override;

    void clear();

protected:
    GLuint m_id;

    Attachments m_attchments;

    std::array<core::graphics::FrameBufferClearColor, core::graphics::FrameBufferColorAttachmentsCount()> m_clearColor;
    float m_clearDepth;
    int32_t m_clearStencil;
    std::unordered_set<core::graphics::FrameBufferAttachment> m_clearMask;
};

class FrameBuffer_4_5 : public FrameBufferBase_4_5
{
    NONCOPYBLE(FrameBuffer_4_5)
public:
    FrameBuffer_4_5();
    ~FrameBuffer_4_5() override;

    void attach(core::graphics::FrameBufferAttachment,
                std::shared_ptr<core::graphics::ISurface>,
                uint32_t level = 0u,
                uint32_t layer = 0u) override;
    void detach(core::graphics::FrameBufferAttachment) override;
};

class DefaultFrameBuffer_4_5 : public FrameBufferBase_4_5
{
    NONCOPYBLE(DefaultFrameBuffer_4_5)
public:
    DefaultFrameBuffer_4_5(GLuint);
    ~DefaultFrameBuffer_4_5() override;

    void attach(core::graphics::FrameBufferAttachment, std::shared_ptr<core::graphics::ISurface>, uint32_t = 0u, uint32_t = 0u) override;
    void detach(core::graphics::FrameBufferAttachment) override;

    void setClearColor(uint32_t, const glm::vec4&) override;
    void setClearColor(uint32_t, const glm::i32vec4&) override;
    void setClearColor(uint32_t, const glm::u32vec4&) override;
    void setClearDepthStencil(float, int32_t) override;
    void setClearMask(const std::unordered_set<core::graphics::FrameBufferAttachment>&) override;
};

class ProgramBase_4_5 : public virtual core::graphics::IProgram
{
    NONCOPYBLE(ProgramBase_4_5)
public:
    ProgramBase_4_5();
    ~ProgramBase_4_5() override;

    GLuint id() const;
    virtual bool preBuild(std::string&);
    virtual bool postBuild(std::string&);

    int32_t uniformLocationByName(const std::string&) const override;

    const std::vector<core::graphics::UniformInfo> &uniformsInfo() const override;
    const std::vector<core::graphics::SSBOInfo> &SSBOsInfo() const override;

    std::string uniformNameByIndex(uint16_t) const override;
    std::string SSBONameByIndex(uint16_t) const override;

protected:
    GLuint m_id;

    std::vector<core::graphics::UniformInfo> m_uniformsInfo;
    std::vector<core::graphics::SSBOInfo> m_SSBOsInfo;

    GLint m_uniformNameMaxLength;
    GLint m_SSBONameMaxLength;
};

class RenderProgram_4_5 : public core::graphics::IRenderProgram, public ProgramBase_4_5
{
    NONCOPYBLE(RenderProgram_4_5)
public:
    RenderProgram_4_5();
    ~RenderProgram_4_5() override;

    bool postBuild(std::string&) override;

    int32_t attributeLocationByName(const std::string&) const override;
    const std::vector<core::graphics::AttributeInfo> &attributesInfo() const override;
    std::string attributeNameByIndex(uint16_t) const override;

protected:
    std::vector<core::graphics::AttributeInfo> m_attributesInfo;

    GLint m_attributeNameMaxLength;
};

class ComputeProgram_4_5 : public core::graphics::IComputeProgram, public ProgramBase_4_5
{
    NONCOPYBLE(ComputeProgram_4_5)
public:
    ComputeProgram_4_5();
    ~ComputeProgram_4_5() override;

    bool postBuild(std::string&) override;

    glm::uvec3 workGroupSize() const override;

protected:
    glm::uvec3 m_workGroupSize;

};

class QtOpenGL_4_5_Renderer : public QOpenGLFunctions_4_5_Core, public core::graphics::IRenderer
{
    NONCOPYBLE(QtOpenGL_4_5_Renderer)
public:
    ~QtOpenGL_4_5_Renderer() override;

    const std::string &name() const override;

    std::shared_ptr<core::graphics::IFrameBuffer> defaultFrameBuffer() override;
    std::shared_ptr<const core::graphics::IFrameBuffer> defaultFrameBuffer() const override;

    void blitFrameBuffer(std::shared_ptr<const core::graphics::IFrameBuffer> src,
                         std::shared_ptr<core::graphics::IFrameBuffer> dst,
                         const glm::uvec4 &srcViewport,
                         const glm::uvec4 &dstViewport,
                         bool colorMsk, bool depthMask, bool stencilMask,
                         bool linearFilter = false) override;

    std::shared_ptr<core::graphics::IBuffer> createBuffer(size_t = 0u,
                                                          const void* = nullptr) const override;
    std::shared_ptr<core::graphics::IBufferRange> createBufferRange(std::shared_ptr<core::graphics::IBuffer>,
                                                                    size_t,
                                                                    size_t = static_cast<size_t>(-1)) const override;
    std::shared_ptr<core::graphics::IVertexArray> createVertexArray(std::shared_ptr<utils::Mesh> = nullptr,
                                                                    bool uniteVertexBuffers = true) const override;
    std::shared_ptr<core::graphics::ITexture> createTexture2DEmpty(uint32_t width,
                                                                   uint32_t height,
                                                                   core::graphics::PixelInternalFormat,
                                                                   uint32_t numLevels = 1) const override;
    std::shared_ptr<core::graphics::ITexture> createTexture2D(std::shared_ptr<utils::Image>,
                                                              core::graphics::PixelInternalFormat = core::graphics::PixelInternalFormat::Undefined,
                                                              uint32_t numLevels = 0,
                                                              bool genMipmaps = true) const override;
    std::shared_ptr<core::graphics::ITexture> createTextureRectEmpty(uint32_t width,
                                                                   uint32_t height,
                                                                   core::graphics::PixelInternalFormat) const override;
    std::shared_ptr<core::graphics::IImage> createImage(std::shared_ptr<core::graphics::ITexture>,
                                                    core::graphics::IImage::DataAccess,
                                                    uint32_t level = 0u) const override;
    std::shared_ptr<core::graphics::IRenderBuffer> createRenderBuffer(uint32_t width,
                                                                      uint32_t height,
                                                                      core::graphics::PixelInternalFormat) const override;
    std::shared_ptr<core::graphics::IFrameBuffer> createFrameBuffer() const override;
    std::shared_ptr<core::graphics::IRenderProgram> createRenderProgram(const std::shared_ptr<utils::Shader> &vertexShader,
                                                                        const std::shared_ptr<utils::Shader> &fragmentShader) const override;
    std::shared_ptr<core::graphics::IComputeProgram> createComputeProgram(const std::shared_ptr<utils::Shader> &computeShader) const override;

    void resize(uint32_t, uint32_t) override;
    const glm::uvec2 &viewportSize() const override;

    void clearRenderData() override;
    void addRenderData(const std::shared_ptr<core::graphics::IRenderProgram>&,
                       const std::shared_ptr<core::IDrawable>&,
                       const glm::mat4x4& = glm::mat4x4(1.f)) override;
    void render(const std::shared_ptr<core::graphics::IFrameBuffer>&, const core::RenderInfo&, const glm::uvec4&) override;
    void compute(const std::shared_ptr<core::graphics::IComputeProgram>&, const core::RenderInfo&, const glm::uvec3&) override;

    int32_t bindTexture(const core::graphics::PTexture&);
    int32_t bindImage(const core::graphics::PImage&);
    void bindBuffer(GLenum target, GLuint bindingPoint, const core::graphics::PBufferRange&);
    uint32_t bindSSBO(const core::graphics::PBufferRange&);
    void bindAtomicCounterBuffer(GLuint bindingPoint, const core::graphics::PBufferRange&);

    static std::shared_ptr<QtOpenGL_4_5_Renderer> create(QOpenGLContext*, GLuint);
    static std::shared_ptr<QtOpenGL_4_5_Renderer> instance(QOpenGLContext* = nullptr);

private:
    QtOpenGL_4_5_Renderer(QOpenGLContext*, GLuint);
    void makeDefaultFrameBuffer(GLuint);

    void setupUniform(GLuint rpId, GLint loc, const core::PAbstratcUniform&);
    void setupUniforms(const std::shared_ptr<ProgramBase_4_5>&,
                       const std::shared_ptr<core::IDrawable>&,
                       const core::RenderInfo&,
                       const glm::mat4&);

    bool createProgram(std::shared_ptr<ProgramBase_4_5>,
                       const std::vector<std::pair<GLenum, std::reference_wrapper<const std::string>>>&) const;

    std::shared_ptr<DefaultFrameBuffer_4_5> m_defaultFrameBuffer;

    std::list<std::tuple<glm::mat4, std::shared_ptr<RenderProgram_4_5>, std::shared_ptr<core::IDrawable>>> m_renderData;
    glm::uvec2 m_viewportSize;
    int32_t m_textureUnit;
    int32_t m_imageUnit;
    uint32_t m_bufferUnit;

    static std::unordered_map<QOpenGLContext*, std::weak_ptr<QtOpenGL_4_5_Renderer>> s_instances;
};

}
}

#endif // QTOPENGL_1_0_RENDERER_H

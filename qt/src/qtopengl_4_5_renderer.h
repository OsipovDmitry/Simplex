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
    static uint16_t GL2PixelNumComponents(GLenum); // for shader vertex attribute
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

class RenderProgram_4_5 : public core::graphics::IRenderProgram
{
    NONCOPYBLE(RenderProgram_4_5)
public:
    RenderProgram_4_5();
    ~RenderProgram_4_5() override;

    GLuint id() const;
    bool compile(std::string&);

    int32_t attributeLocationByName(const std::string&) const override;
    int32_t uniformLocationByName(const std::string&) const override;

    const std::vector<core::graphics::AttributeInfo> &attributesInfo() const override;
    const std::vector<core::graphics::UniformInfo> &uniformsInfo() const override;

    std::string attributeNameByIndex(uint16_t) const override;
    std::string uniformNameByIndex(uint16_t) const override;

private:
    GLuint m_id = 0;

    std::vector<core::graphics::AttributeInfo> m_attributesInfo;
    std::vector<core::graphics::UniformInfo> m_uniformsInfo;

    GLint m_attributeNameMaxLength;
    GLint m_uniformNameMaxLength;
};



class QtOpenGL_4_5_Renderer : public QOpenGLFunctions_4_5_Core, public core::graphics::IRenderer
{
    NONCOPYBLE(QtOpenGL_4_5_Renderer)
public:
    ~QtOpenGL_4_5_Renderer() override;

    const std::string &name() const override;

    std::shared_ptr<core::graphics::IFrameBuffer> defaultFrameBuffer() override;
    std::shared_ptr<const core::graphics::IFrameBuffer> defaultFrameBuffer() const override;

    std::shared_ptr<core::graphics::IRenderProgram> createRenderProgram(const std::string &vertexShader,
                                                                        const std::string &fragmentShader) const override;
    std::shared_ptr<core::graphics::IBuffer> createBuffer(size_t = 0u,
                                                          const void* = nullptr) const override;
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
    std::shared_ptr<core::graphics::IRenderBuffer> createRenderBuffer(uint32_t width,
                                                                      uint32_t height,
                                                                      core::graphics::PixelInternalFormat) const override;
    std::shared_ptr<core::graphics::IFrameBuffer> createFrameBuffer() const override;

    void resize(uint32_t, uint32_t) override;
    const glm::uvec2 &viewportSize() const override;

    void clearRenderData() override;
    void addRenderData(const glm::mat4&, std::shared_ptr<core::IDrawable>) override;
    void render(const core::RenderInfo&) override;

    int32_t bindTexture(std::shared_ptr<const core::graphics::ITexture>);

    static std::shared_ptr<QtOpenGL_4_5_Renderer> create(QOpenGLContext*, GLuint);
    static std::shared_ptr<QtOpenGL_4_5_Renderer> instance(QOpenGLContext* = nullptr);

private:
    QtOpenGL_4_5_Renderer(QOpenGLContext*, GLuint);
    void makeDefaultFrameBuffer(GLuint);

    void setupUniform(GLuint, GLint, std::shared_ptr<const core::AbstractUniform>);
    void setupUniforms(std::shared_ptr<core::IDrawable>, const core::RenderInfo&, const glm::mat4&);

    std::shared_ptr<DefaultFrameBuffer_4_5> m_defaultFrameBuffer;

    std::list<std::pair<glm::mat4, std::shared_ptr<core::IDrawable>>> m_renderData;
    glm::uvec2 m_viewportSize;
    int32_t m_textureUnit;

    static std::unordered_map<QOpenGLContext*, std::weak_ptr<QtOpenGL_4_5_Renderer>> s_instances;
};

}
}

#endif // QTOPENGL_1_0_RENDERER_H

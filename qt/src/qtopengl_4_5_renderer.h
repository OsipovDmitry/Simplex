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

class QtOpenGL_4_5_Renderer : public core::IGraphicsRenderer, public QOpenGLFunctions_4_5_Core
{
    NONCOPYBLE(QtOpenGL_4_5_Renderer)
public:

    class Buffer_4_5 : public core::IGraphicsRenderer::Buffer, public std::enable_shared_from_this<Buffer_4_5>
    {
        NONCOPYBLE(Buffer_4_5)
    public:
        class MappedData_4_5 : public core::IGraphicsRenderer::Buffer::MappedData
        {
        public:
            MappedData_4_5(std::shared_ptr<const Buffer_4_5>, uint8_t*);
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

        static GLbitfield MapAccess2GL(MapAccess);
        friend class MappedData_4_5;
    };

    class VertexArray_4_5 : public core::IGraphicsRenderer::VertexArray
    {
        NONCOPYBLE(VertexArray_4_5)
    public:
        VertexArray_4_5();
        ~VertexArray_4_5() override;

        GLuint id() const;

        uint32_t attachVertexBuffer(std::shared_ptr<Buffer> buffer, size_t offset, uint32_t stride) override;
        void detachVertexBuffer(uint32_t bindingIndex) override;
        std::shared_ptr<const Buffer> vertexBuffer(uint32_t bindingIndex) const override;
        size_t vertexBufferOffset(uint32_t bindingIndex) const override;
        uint32_t vertexBufferStride(uint32_t bindingIndex) const override;

        void declareVertexAttribute(utils::VertexAttribute, uint32_t bindingIndex, uint32_t numComponents, utils::Type type, uint32_t relativeOffset) override;
        void undeclareVertexAttribute(utils::VertexAttribute) override;
        uint32_t vertexAttributeBindingIndex(utils::VertexAttribute) const override;
        uint32_t vertexAttributeNumComponents(utils::VertexAttribute) const override;
        utils::Type vertexAttributeType(utils::VertexAttribute) const override;
        uint32_t vertexAttributeRelativeOffset(utils::VertexAttribute) const override;

        void attachIndexBuffer(std::shared_ptr<Buffer> buffer) override;
        void detachIndexBuffer() override;
        std::shared_ptr<const Buffer> indexBuffer() const override;

        void addPrimitiveSet(std::shared_ptr<utils::PrimitiveSet>) override;
        void removePrimitiveSet(std::shared_ptr<utils::PrimitiveSet>) override;
        const std::unordered_set<std::shared_ptr<utils::PrimitiveSet>> primitiveSets() const override;

    private:
        GLuint m_id = 0;

        std::vector<std::tuple<std::shared_ptr<Buffer_4_5>, size_t, uint32_t>> m_vertexBuffers;
        std::unordered_map<utils::VertexAttribute, std::tuple<uint32_t, uint32_t, utils::Type, uint32_t>> m_vertexDeclarations;

        std::shared_ptr<Buffer_4_5> m_indexBuffer;

        std::unordered_set<std::shared_ptr<utils::PrimitiveSet>> m_primitiveSets;
    };

    class TextureBase_4_5 : public Texture
    {
    public:
        TextureBase_4_5();
        ~TextureBase_4_5() override;

        GLuint id() const;

        glm::uvec3 size(uint32_t level = 0) const override;
        uint32_t numMipmapLevels() const override;
        InternalFormat internalFormat() const override;

        void generateMipmaps() override;

    protected:
        GLuint m_id = 0;

        static GLenum InternalFormat2GL(InternalFormat);
        static InternalFormat GL2InternalFormat(GLenum);
        static GLenum NumComponents2GL(uint32_t);
    };

    class Texture2D_4_5 : public TextureBase_4_5
    {
    public:
        Texture2D_4_5(uint32_t width, uint32_t height, Texture::InternalFormat, uint32_t numLevels);
        ~Texture2D_4_5() override = default;

        void setSubImage(uint32_t level,
                         const glm::uvec3 &offset,
                         const glm::uvec3 &size,
                         uint32_t numComponents,
                         utils::Type type,
                         const void *data) override;

    };

    class RenderProgram_4_5 : public core::IGraphicsRenderer::RenderProgram
    {
        NONCOPYBLE(RenderProgram_4_5)
    public:
        RenderProgram_4_5();
        ~RenderProgram_4_5() override;

        GLuint id() const;
        bool compile(std::string&);

        const std::vector<AttributeInfo> &attributesInfo() const override;
        const std::vector<UniformInfo> &uniformsInfo() const override;

        std::string attributeNameByIndex(uint16_t) const override;
        std::string uniformNameByIndex(uint16_t) const override;

        void setUniform(int32_t, float) override;
        void setUniform(int32_t, const glm::vec2&) override;
        void setUniform(int32_t, const glm::vec3&) override;
        void setUniform(int32_t, const glm::vec4&) override;

        void setUniform(int32_t, int32_t) override;
        void setUniform(int32_t, const glm::ivec2&) override;
        void setUniform(int32_t, const glm::ivec3&) override;
        void setUniform(int32_t, const glm::ivec4&) override;

        void setUniform(int32_t, uint32_t) override;
        void setUniform(int32_t, const glm::uvec2&) override;
        void setUniform(int32_t, const glm::uvec3&) override;
        void setUniform(int32_t, const glm::uvec4&) override;

        void setUniform(int32_t, const glm::mat2&) override;
        void setUniform(int32_t, const glm::mat3&) override;
        void setUniform(int32_t, const glm::mat4&) override;

        void setUniform(int32_t, std::shared_ptr<const Texture>) override;

    private:
        GLuint m_id = 0;

        std::vector<AttributeInfo> m_attributesInfo;
        std::vector<UniformInfo> m_uniformsInfo;

        GLint m_attributeNameMaxLength;
        GLint m_uniformNameMaxLength;
    };


    std::shared_ptr<RenderProgram> createRenderProgram(const std::string &vertexShader, const std::string &fragmentShader) const override;
    std::shared_ptr<Buffer> createBuffer(size_t = 0u, const void* = nullptr) const override;
    std::shared_ptr<VertexArray> createVertexArray(std::shared_ptr<utils::Mesh> = nullptr, bool uniteVertexBuffers = true) const override;
    std::shared_ptr<Texture> createTexture2DEmpty(uint32_t width, uint32_t height, Texture::InternalFormat, uint32_t numLevels = 1) const override;
    std::shared_ptr<Texture> createTexture2D(std::shared_ptr<utils::Image>,
                                             Texture::InternalFormat = Texture::InternalFormat::Undefined,
                                             uint32_t numLevels = 0, bool genMipmaps = true) const override;

    ~QtOpenGL_4_5_Renderer() override;
    static std::shared_ptr<QtOpenGL_4_5_Renderer> instance();

    const std::string &name() const override;

    void resize(uint32_t, uint32_t) override;
    const glm::uvec2 &viewportSize() const override;

    void clearRenderData() override;
    void addRenderData(const glm::mat4&, std::shared_ptr<core::IDrawable>) override;
    void render(const core::RenderInfo&) override;

    uint32_t bindTexture(std::shared_ptr<const Texture>);

private:
    static std::weak_ptr<QtOpenGL_4_5_Renderer> s_instance;

    std::list<std::pair<glm::mat4, std::shared_ptr<core::IDrawable>>> m_renderData;
    glm::uvec2 m_viewportSize;
    uint32_t m_textureUnit;

    QtOpenGL_4_5_Renderer(const QOpenGLContext*);
    static void setInstance(std::shared_ptr<QtOpenGL_4_5_Renderer>);

    static GLenum Type2GL(utils::Type);
    static utils::Type GL2Type(GLenum);
    static GLenum PrimitiveType2GL(utils::PrimitiveType);
    static Texture::InternalFormat NumComponentsAndTypeToInternalFormat(uint32_t, utils::Type);
    static void setupUniforms(std::shared_ptr<core::IDrawable>, const core::RenderInfo&, const glm::mat4&);
    friend class QtRenderWidget;
};

}
}

#endif // QTOPENGL_1_0_RENDERER_H

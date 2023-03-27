#ifndef QTOPENGL_1_0_RENDERER_H
#define QTOPENGL_1_0_RENDERER_H

#include <unordered_map>
#include <tuple>

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
    class RenderProgram_4_5 : public core::IGraphicsRenderer::RenderProgram
    {
        NONCOPYBLE(RenderProgram_4_5)
    public:
        RenderProgram_4_5();
        ~RenderProgram_4_5() override;

        GLuint id() const;

        std::vector<std::string> attributesInfo() override;
        std::vector<std::string> uniformsInfo() override;

        int32_t attributeLocation(const std::string&) override;
        int32_t uniformLocation(const std::string&) override;

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

        void setUniform(int32_t, const glm::mat2x2&) override;
        void setUniform(int32_t, const glm::mat3x3&) override;
        void setUniform(int32_t, const glm::mat4x4&) override;

    private:
        GLuint m_id = 0;
    };

    class Buffer_4_5 : public core::IGraphicsRenderer::Buffer, std::enable_shared_from_this<Buffer_4_5>
    {
        NONCOPYBLE(Buffer_4_5)
    public:
        Buffer_4_5(uint64_t size, const void *data);
        ~Buffer_4_5() override;

        GLuint id() const;

        size_t size() override;
        void resize(size_t size, const void *data) override;

        class MappedData_4_5 : public core::IGraphicsRenderer::Buffer::MappedData
        {
        public:
            MappedData_4_5(std::shared_ptr<Buffer_4_5>, void*);
            ~MappedData_4_5() override;
            void *get() override;
        private:
            std::weak_ptr<Buffer_4_5> m_mappedBuffer;
            void *m_data;
        };

        std::unique_ptr<MappedData> map(MapAccess access, size_t offset = 0u, size_t size = 0u) override;

    private:
        GLuint m_id = 0;
        bool m_isMapped = false;

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
        std::shared_ptr<Buffer> vertexBufferByBindingIndex(uint32_t bindingIndex) override;
        size_t offsetByBindingIndex(uint32_t bindingIndex) override;
        uint32_t strideByBindingIndex(uint32_t bindingIndex) override;

        void attachIndexBuffer(std::shared_ptr<Buffer> buffer, utils::PrimitiveType primitiveType, uint32_t count, utils::Type type) override;
        void detachIndexBuffer() override;
        std::shared_ptr<Buffer> indexBuffer() override;
        utils::PrimitiveType primitiveType() override;
        uint32_t elementsCount() override;
        utils::Type indicesType() override;

        void declareVertexAttribute(utils::VertexAttribute, uint32_t bindingIndex, uint8_t numComponents, utils::Type type, uint32_t relativeOffset) override;
        void undeclareVertexAttribute(utils::VertexAttribute) override;
        uint32_t bindingIndexByVertexAttribute(utils::VertexAttribute) override;
        uint8_t numComponentsByVertexAttribute(utils::VertexAttribute) override;
        utils::Type typeByVertexAttribute(utils::VertexAttribute) override;
        uint32_t relativeOffsetByVertexAttribute(utils::VertexAttribute) override;

    private:
        GLuint m_id = 0;

        std::vector<std::tuple<std::shared_ptr<Buffer_4_5>, size_t, uint32_t>> m_vertexBuffers;
        std::tuple<std::shared_ptr<Buffer_4_5>, utils::PrimitiveType, uint32_t, utils::Type> m_indexBuffer;
        std::unordered_map<utils::VertexAttribute, std::tuple<uint32_t, uint8_t, utils::Type, uint32_t>> m_vertexDeclarations;

    };

    std::shared_ptr<RenderProgram> createRenderProgram(const std::string& vertexShader, const std::string& fragmentShader) override;
    std::shared_ptr<Buffer> createBuffer(size_t = 0u, const void* = nullptr) override;
    std::shared_ptr<VertexArray> createVertexArray() override;


    ~QtOpenGL_4_5_Renderer() override;
    static std::shared_ptr<QtOpenGL_4_5_Renderer> instance();

    const std::string &name() const override;

    void resize(int, int);
    void render();

private:
    static std::weak_ptr<QtOpenGL_4_5_Renderer> s_instance;

    QtOpenGL_4_5_Renderer(const QOpenGLContext*);
    static void setInstance(std::shared_ptr<QtOpenGL_4_5_Renderer>);

    static GLenum Type2GL(utils::Type);
    static GLenum PrimitiveType2GL(utils::PrimitiveType);

    friend class QtRenderWidget;
};

}
}

#endif // QTOPENGL_1_0_RENDERER_H

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

#include <utils/forwarddecl.h>
#include <utils/enumclass.h>
#include <utils/types.h>

#include <core/forwarddecl.h>
#include <core/inamedobject.h>

namespace simplex
{
namespace core
{

class IGraphicsRenderer : public INamedObject
{
public:

    class Buffer
    {
    public:
        class MappedData {
        public:
            virtual ~MappedData() = default; // mapped data should be unmapped in destructor's implementation
            virtual const uint8_t *get() const = 0;
            virtual uint8_t *get() = 0;
        };

        virtual ~Buffer() = default;

        virtual size_t size() const = 0;
        virtual void resize(size_t size, const void *data) = 0;

        ENUMCLASS(MapAccess, uint8_t, ReadOnly, WriteOnly, ReadWrite)
        virtual std::unique_ptr<const MappedData> map(MapAccess access, size_t offset = 0u, size_t size = 0u) const = 0;
        virtual std::unique_ptr<MappedData> map(MapAccess access, size_t offset = 0u, size_t size = 0u) = 0;
    };

    class VertexArray
    {
    public:
        virtual ~VertexArray() = default;

        virtual uint32_t attachVertexBuffer(std::shared_ptr<Buffer> buffer, size_t offset, uint32_t stride) = 0; // returns bindingIndex
        virtual void detachVertexBuffer(uint32_t bindingIndex) = 0;
        virtual std::shared_ptr<const Buffer> vertexBuffer(uint32_t bindingIndex) const = 0;
        virtual size_t vertexBufferOffset(uint32_t bindingIndex) const = 0;
        virtual uint32_t vertexBufferStride(uint32_t bindingIndex) const = 0;

        virtual void declareVertexAttribute(utils::VertexAttribute, uint32_t bindingIndex, uint32_t numComponents, utils::Type type, uint32_t relativeOffset) = 0;
        virtual void undeclareVertexAttribute(utils::VertexAttribute) = 0;
        virtual uint32_t vertexAttributeBindingIndex(utils::VertexAttribute) const = 0;
        virtual uint32_t vertexAttributeNumComponents(utils::VertexAttribute) const = 0;
        virtual utils::Type vertexAttributeType(utils::VertexAttribute) const = 0;
        virtual uint32_t vertexAttributeRelativeOffset(utils::VertexAttribute) const = 0;

        virtual void attachIndexBuffer(std::shared_ptr<Buffer> buffer) = 0;
        virtual void detachIndexBuffer() = 0;
        virtual std::shared_ptr<const Buffer> indexBuffer() const = 0;

        virtual void addPrimitiveSet(std::shared_ptr<utils::PrimitiveSet>) = 0;
        virtual void removePrimitiveSet(std::shared_ptr<utils::PrimitiveSet>) = 0;
        virtual const std::unordered_set<std::shared_ptr<utils::PrimitiveSet>> primitiveSets() const = 0;
    };

    class RenderSurface
    {
    public:
        ENUMCLASS(InternalFormat, uint32_t,
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

        virtual ~RenderSurface() = default;

        virtual glm::uvec2 size() const = 0;
        virtual InternalFormat internalFormat() const = 0;
    };

    class Texture : public RenderSurface
    {
    public:
        ENUMCLASS(WrapMode, uint32_t,
                  ClampToEdge,
                  ClampToBorder,
                  MirroredRepeat,
                  Repeat,
                  MirrorClampToEdge)

        ENUMCLASS(Filtering, uint32_t,
                  Point,
                  Linear,
                  Bilinear,
                  Trilinear)

        virtual glm::uvec3 mipmapSize(uint32_t level = 0) const = 0;
        virtual uint32_t numMipmapLevels() const = 0;

        virtual void setSubImage(uint32_t level,
                                 const glm::uvec3 &offset,
                                 const glm::uvec3 &size,
                                 uint32_t numComponents,
                                 utils::Type type,
                                 const void *data) = 0;

        virtual void generateMipmaps() = 0;
        virtual void setBorderColor(const glm::vec4&) = 0;
        virtual void setWrapMode(WrapMode) = 0;
        virtual void setFiltering(Filtering) = 0;

    };

    class RenderBuffer : public RenderSurface
    {
    public:
    };

    class FrameBuffer
    {
    public:
        ENUMCLASS(Attachment, uint32_t,
                  Color0,
                  Color1,
                  Color2,
                  Color3,
                  Depth,
                  Stencil,
                  DepthStencil)

        static constexpr Attachment ColorAttachment(uint32_t i) { return castToAttachment(castFromAttachment(Attachment::Color0) + i); }
        static constexpr uint32_t ColorAttachmentIndex(Attachment a) { return castFromAttachment(a) - castFromAttachment(Attachment::Color0); }
        static constexpr uint32_t ColorAttachmentsCount() { return ColorAttachmentIndex(Attachment::Depth); }

        struct AttachmentInfo
        {
            std::shared_ptr<RenderSurface> renderSurface;
            uint32_t level;
            uint32_t layer;
        };

        using Attachments = std::unordered_map<Attachment, AttachmentInfo>;

        virtual ~FrameBuffer() = default;

        virtual bool isComplete() const = 0;

        virtual const Attachments &attachments() const = 0;
        virtual void attach(Attachment, std::shared_ptr<RenderSurface>, uint32_t level = 0u, uint32_t layer = 0u) = 0;
        virtual void detach(Attachment) = 0;

        union ClearColorValue
        {
            glm::vec4 floatColor;
            glm::i32vec4 intColor;
            glm::u32vec4 uintColor;
        };
        using ClearColor = std::pair<utils::Type, ClearColorValue>;

        virtual const ClearColor &clearColor(uint32_t) const = 0;
        virtual void setClearColor(uint32_t, const glm::vec4&) = 0;
        virtual void setClearColor(uint32_t, const glm::i32vec4&) = 0;
        virtual void setClearColor(uint32_t, const glm::u32vec4&) = 0;

        virtual float clearDepth() const = 0;
        virtual int32_t clearStencil() const = 0;
        virtual void setClearDepthStencil(float, int32_t) = 0;
    };

    class RenderProgram
    {
    public:
        ENUMCLASS(UniformId, uint16_t,
                  Viewport,
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
                  ViewPosition,
                  ViewXDirection,
                  ViewYDirection,
                  ViewZDirection)

        struct UniformInfo
        {
            UniformId id;
            uint16_t index;
            int32_t location;
            utils::Type type;
        };

        struct AttributeInfo
        {
            utils::VertexAttribute id;
            uint16_t index;
            int32_t location;
            utils::Type type;
        };

        virtual ~RenderProgram() = default;

        virtual const std::vector<AttributeInfo> &attributesInfo() const = 0;
        virtual const std::vector<UniformInfo> &uniformsInfo() const = 0;

        virtual std::string attributeNameByIndex(uint16_t) const = 0;
        virtual std::string uniformNameByIndex(uint16_t) const = 0;

        virtual void setUniform(int32_t, float) = 0;
        virtual void setUniform(int32_t, const glm::vec2&) = 0;
        virtual void setUniform(int32_t, const glm::vec3&) = 0;
        virtual void setUniform(int32_t, const glm::vec4&) = 0;

        virtual void setUniform(int32_t, int32_t) = 0;
        virtual void setUniform(int32_t, const glm::ivec2&) = 0;
        virtual void setUniform(int32_t, const glm::ivec3&) = 0;
        virtual void setUniform(int32_t, const glm::ivec4&) = 0;

        virtual void setUniform(int32_t, uint32_t) = 0;
        virtual void setUniform(int32_t, const glm::uvec2&) = 0;
        virtual void setUniform(int32_t, const glm::uvec3&) = 0;
        virtual void setUniform(int32_t, const glm::uvec4&) = 0;

        virtual void setUniform(int32_t, const glm::mat2&) = 0;
        virtual void setUniform(int32_t, const glm::mat3&) = 0;
        virtual void setUniform(int32_t, const glm::mat4&) = 0;

        virtual void setUniform(int32_t, std::shared_ptr<const Texture>) = 0;

    protected:
        static utils::VertexAttribute vertexAttributeByName(const std::string&);
        static UniformId UniformIdByName(const std::string&);

        static utils::Type typeByUniformId(UniformId);
    };


    virtual std::shared_ptr<FrameBuffer> defaultFrameBuffer() = 0;
    virtual std::shared_ptr<const FrameBuffer> defaultFrameBuffer() const = 0;

    virtual std::shared_ptr<RenderProgram> createRenderProgram(const std::string &vertexShader, const std::string &fragmentShader) const = 0;
    virtual std::shared_ptr<Buffer> createBuffer(size_t size = 0u, const void *data = nullptr) const = 0;
    virtual std::shared_ptr<VertexArray> createVertexArray(std::shared_ptr<utils::Mesh> = nullptr, bool uniteBuffers = true) const = 0;
    virtual std::shared_ptr<Texture> createTexture2DEmpty(uint32_t width, uint32_t height, RenderSurface::InternalFormat, uint32_t numLevels = 1) const = 0;
    virtual std::shared_ptr<Texture> createTexture2D(std::shared_ptr<utils::Image>,
                                                     RenderSurface::InternalFormat = RenderSurface::InternalFormat::Undefined,
                                                     uint32_t numLevels = 0, bool genMipmaps = true) const = 0;
    virtual std::shared_ptr<RenderBuffer> createRenderBuffer(uint32_t width, uint32_t height, RenderSurface::InternalFormat) const = 0;
    virtual std::shared_ptr<FrameBuffer> createFrameBuffer() const = 0;

    virtual void resize(uint32_t, uint32_t) = 0;
    virtual const glm::uvec2 &viewportSize() const = 0;

    virtual void clearRenderData() = 0;
    virtual void addRenderData(const glm::mat4&, std::shared_ptr<IDrawable>) = 0;
    virtual void render(const RenderInfo&) = 0;
};

inline utils::VertexAttribute IGraphicsRenderer::RenderProgram::vertexAttributeByName(const std::string &name)
{
    static const std::unordered_map<std::string, utils::VertexAttribute> s_table {
        { "a_position", utils::VertexAttribute::Position },
        { "a_normal", utils::VertexAttribute::Normal },
        { "a_texCoord", utils::VertexAttribute::TexCoord },
        { "a_boneIDs", utils::VertexAttribute::BonesIDs },
        { "a_bonesWeights", utils::VertexAttribute::BonesWeights },
        { "a_tangent", utils::VertexAttribute::Tangent },
        { "a_color", utils::VertexAttribute::Color },
    };

    auto it = s_table.find(name);
    return (it == s_table.end()) ? utils::VertexAttribute::Count : it->second;
}

inline IGraphicsRenderer::RenderProgram::UniformId IGraphicsRenderer::RenderProgram::UniformIdByName(const std::string &name)
{
    static const std::unordered_map<std::string, UniformId> s_table {
        { "u_viewport", UniformId::Viewport },
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
        { "u_viewPosition", UniformId::ViewPosition },
        { "u_viewXDirection", UniformId::ViewXDirection },
        { "u_viewYDirection", UniformId::ViewYDirection },
        { "u_viewZDirection", UniformId::ViewZDirection },
    };

    auto it = s_table.find(name);
    return (it == s_table.end()) ? UniformId::Count : it->second;
}

inline utils::Type IGraphicsRenderer::RenderProgram::typeByUniformId(UniformId uniformId)
{
    static const std::array<utils::Type, numElementsUniformId() + 1u> s_table {
        utils::Type::Uint32Vec2,
        utils::Type::SingleMat4,
        utils::Type::SingleMat4,
        utils::Type::SingleMat4,
        utils::Type::SingleMat4,
        utils::Type::SingleMat4,
        utils::Type::SingleMat4,
        utils::Type::SingleMat4,
        utils::Type::SingleMat4,
        utils::Type::SingleMat4,
        utils::Type::SingleMat4,
        utils::Type::SingleMat4,
        utils::Type::SingleVec3,
        utils::Type::SingleVec3,
        utils::Type::SingleVec3,
        utils::Type::SingleVec3,
        utils::Type::Undefined
    };

    return s_table[castFromUniformId(uniformId)];
}

}
}

#endif // IGRAPHICSRENDERER_H

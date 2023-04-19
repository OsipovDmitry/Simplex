#ifndef IGRAPHICSRENDERER_H
#define IGRAPHICSRENDERER_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_set>

#include <utils/glm/vec2.hpp>
#include <utils/glm/vec3.hpp>
#include <utils/glm/vec4.hpp>
#include <utils/glm/mat2x2.hpp>
#include <utils/glm/mat3x3.hpp>
#include <utils/glm/mat4x4.hpp>

#include <utils/enumclass.h>
#include <utils/forwarddecl.h>

#include <core/forwarddecl.h>
#include <core/inamedobject.h>

namespace simplex
{
namespace core
{

class IGraphicsRenderer : public INamedObject
{
public:
    class RenderProgram
    {
    public:
        virtual ~RenderProgram() = default;

        virtual std::vector<std::string> attributesInfo() const = 0;
        virtual std::vector<std::string> uniformsInfo() const = 0;

        virtual int32_t attributeLocation(const std::string&) const = 0;
        virtual int32_t uniformLocation(const std::string&) const = 0;

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
    };

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

    virtual std::shared_ptr<RenderProgram> createRenderProgram(const std::string& vertexShader, const std::string& fragmentShader) const = 0;
    virtual std::shared_ptr<Buffer> createBuffer(size_t size = 0u, const void *data = nullptr) const = 0;
    virtual std::shared_ptr<VertexArray> createVertexArray(std::shared_ptr<utils::Mesh> = nullptr, bool uniteBuffers = true) const = 0;

    virtual void resize(uint32_t, uint32_t) = 0;
    virtual uint32_t width() const = 0;
    virtual uint32_t height() const = 0;

    virtual void clearRenderData() = 0;
    virtual void addRenderData(const glm::mat4&, std::shared_ptr<IDrawable>) = 0;
    virtual void render(const RenderInfo&) = 0;
};

}
}

#endif // IGRAPHICSRENDERER_H

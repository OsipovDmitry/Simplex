#ifndef IGRAPHICSRENDERER_H
#define IGRAPHICSRENDERER_H

#include <memory>
#include <string>
#include <vector>

#include <utils/glm/vec2.hpp>
#include <utils/glm/vec3.hpp>
#include <utils/glm/vec4.hpp>
#include <utils/glm/mat2x2.hpp>
#include <utils/glm/mat3x3.hpp>
#include <utils/glm/mat4x4.hpp>

#include <utils/enumclass.h>
#include <utils/forwarddecl.h>

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

        virtual std::vector<std::string> attributesInfo() = 0;
        virtual std::vector<std::string> uniformsInfo() = 0;

        virtual int32_t attributeLocation(const std::string&) = 0;
        virtual int32_t uniformLocation(const std::string&) = 0;

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

        virtual void setUniform(int32_t, const glm::mat2x2&) = 0;
        virtual void setUniform(int32_t, const glm::mat3x3&) = 0;
        virtual void setUniform(int32_t, const glm::mat4x4&) = 0;
    };

    class Buffer
    {
    public:
        virtual ~Buffer() = default;

        virtual size_t size() = 0;
        virtual void resize(size_t size, const void *data) = 0;

        class MappedData {
        public:
            virtual ~MappedData() = default; // mapped data should be unmapped in destructor's implementation
            virtual void *get() = 0;
        };
        ENUMCLASS(MapAccess, uint8_t, OnlyRead, OnlyWrite, ReadWrite)
        virtual std::unique_ptr<MappedData> map(MapAccess access, size_t offset = 0u, size_t size = 0u) = 0;
    };

    class VertexArray
    {
    public:
        virtual ~VertexArray() = default;

        virtual uint32_t attachVertexBuffer(std::shared_ptr<Buffer> buffer, size_t offset, uint32_t stride) = 0; // returns bindingIndex
        virtual void detachVertexBuffer(uint32_t bindingIndex) = 0;
        virtual std::shared_ptr<Buffer> vertexBufferByBindingIndex(uint32_t bindingIndex) = 0;
        virtual size_t offsetByBindingIndex(uint32_t bindingIndex) = 0;
        virtual uint32_t strideByBindingIndex(uint32_t bindingIndex) = 0;

        virtual void attachIndexBuffer(std::shared_ptr<Buffer> buffer, utils::PrimitiveType primitiveType, uint32_t count, utils::Type type) = 0;
        virtual void detachIndexBuffer() = 0;
        virtual std::shared_ptr<Buffer> indexBuffer() = 0;
        virtual utils::PrimitiveType primitiveType() = 0;
        virtual uint32_t elementsCount() = 0;
        virtual utils::Type indicesType() = 0;

        virtual void declareVertexAttribute(utils::VertexAttribute, uint32_t bindingIndex, uint8_t numComponents, utils::Type type, uint32_t relativeOffset) = 0;
        virtual void undeclareVertexAttribute(utils::VertexAttribute) = 0;
        virtual uint32_t bindingIndexByVertexAttribute(utils::VertexAttribute) = 0;
        virtual uint8_t numComponentsByVertexAttribute(utils::VertexAttribute) = 0;
        virtual utils::Type typeByVertexAttribute(utils::VertexAttribute) = 0;
        virtual uint32_t relativeOffsetByVertexAttribute(utils::VertexAttribute) = 0;
    };

    virtual std::shared_ptr<RenderProgram> createRenderProgram(const std::string& vertexShader, const std::string& fragmentShader) = 0;
    virtual std::shared_ptr<Buffer> createBuffer(size_t size = 0u, const void *data = nullptr) = 0;
    virtual std::shared_ptr<VertexArray> createVertexArray() = 0;
};

}
}

#endif // IGRAPHICSRENDERER_H

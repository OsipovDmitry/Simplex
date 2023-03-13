#ifndef IGRAPHICSRENDERER_H
#define IGRAPHICSRENDERER_H

#include <memory>
#include <string>

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
    };

    class Buffer
    {
    public:
        virtual ~Buffer() = default;

        virtual size_t size() = 0;
        virtual void resize(size_t size, const void *data) = 0;

        class MappedData {
        public:
            virtual ~MappedData() = default; // mapped data should be unmapped in implementation
            virtual void *get() = 0;
        };
        ENUMCLASS(MapAccess, uint8_t, OnlyRead, OnlyWrite, ReadWrite)
        virtual std::unique_ptr<MappedData> map(MapAccess access, size_t offset = 0u, size_t size = 0u) = 0;
    };

    class VertexArray
    {
    public:
        virtual ~VertexArray() = default;

        virtual void attachVertexBuffer(utils::VertexAttribute, std::shared_ptr<Buffer>, uint8_t, utils::Type, size_t, size_t, uint32_t) = 0;
        virtual void detachVertexBuffer(utils::VertexAttribute) = 0;
        virtual std::shared_ptr<Buffer> vertexBuffer(utils::VertexAttribute) const = 0;

        virtual void setDrawDataDescription(utils::PrimitiveType, size_t, std::shared_ptr<Buffer>, utils::Type) = 0;
        virtual utils::PrimitiveType primitiveType() const = 0;
        virtual size_t elementsCount() const = 0;
        virtual std::shared_ptr<Buffer> indexBuffer() const = 0;
        virtual utils::Type indicesType() const = 0;
    };

    virtual std::shared_ptr<RenderProgram> createRenderProgram(const std::string& vertexShader, const std::string& fragmentShader) = 0;
    virtual std::shared_ptr<Buffer> createBuffer(size_t size = 0u, const void *data = nullptr) = 0;
    virtual std::shared_ptr<VertexArray> createVertexArray() = 0;
};

}
}

#endif // IGRAPHICSRENDERER_H

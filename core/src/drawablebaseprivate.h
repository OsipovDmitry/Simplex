#ifndef CORE_DRAWABLEBASEPRIVATE_H
#define CORE_DRAWABLEBASEPRIVATE_H

#include <memory>

#include <utils/glm/vec3.hpp>
#include <utils/types.h>

#include <core/igraphicsrenderer.h>

namespace simplex
{
namespace core
{

class DrawableBasePrivate
{
public:
    DrawableBasePrivate(std::shared_ptr<IGraphicsRenderer::RenderProgram> rp, std::shared_ptr<IGraphicsRenderer::VertexArray> vao)
        : m_renderProgram(rp)
        , m_vertexArray(vao)
    {}

    std::shared_ptr<IGraphicsRenderer::RenderProgram> renderProgram() { return m_renderProgram; }
    std::shared_ptr<IGraphicsRenderer::VertexArray> vertexArray() { return m_vertexArray; }

    template<typename T>
    static T readDataToType(const uint8_t* data, utils::Type type)
    {
        T result(0u);
        switch (type)
        {
        case utils::Type::Single:
        {
            result = static_cast<T>(*reinterpret_cast<const float*>(data));
            break;
        }
        case utils::Type::Double:
        {
            result = static_cast<T>(*reinterpret_cast<const double*>(data));
            break;
        }
        case utils::Type::Int8:
        {
            result = static_cast<T>(*reinterpret_cast<const int8_t*>(data));
            break;
        }
        case utils::Type::Int16:
        {
            result = static_cast<T>(*reinterpret_cast<const int16_t*>(data));
            break;
        }
        case utils::Type::Int32:
        {
            result = static_cast<T>(*reinterpret_cast<const int32_t*>(data));
            break;
        }
        case utils::Type::Uint8:
        {
            result = static_cast<T>(*reinterpret_cast<const uint8_t*>(data));
            break;
        }
        case utils::Type::Uint16:
        {
            result = static_cast<T>(*reinterpret_cast<const uint16_t*>(data));
            break;
        }
        case utils::Type::Uint32:
        {
            result = static_cast<T>(*reinterpret_cast<const uint32_t*>(data));
            break;
        }
        default:
            break;
        }
        return result;
    }

    template<uint32_t VecLength, typename T>
    static glm::vec<VecLength, T> readDataToVec(const uint8_t* data, uint32_t numComponents, utils::Type type, uint32_t typeSize)
    {
        numComponents = glm::max(numComponents, VecLength);

        glm::vec<VecLength, T> result(0.f);
        for (uint32_t i = 0u; i < numComponents; ++i)
            result[static_cast<int>(i)] = readDataToType<T>(data + i * typeSize, type);
        return result;
    }

private:
    std::shared_ptr<IGraphicsRenderer::RenderProgram> m_renderProgram;
    std::shared_ptr<IGraphicsRenderer::VertexArray> m_vertexArray;

};

}
}

#endif // CORE_DRAWABLEBASEPRIVATE_H

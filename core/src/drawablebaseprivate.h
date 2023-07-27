#ifndef CORE_DRAWABLEBASEPRIVATE_H
#define CORE_DRAWABLEBASEPRIVATE_H

#include <memory>
#include <unordered_map>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class DrawableBasePrivate
{
public:
    DrawableBasePrivate(std::shared_ptr<graphics::IRenderProgram> rp, std::shared_ptr<graphics::IVertexArray> vao);

    virtual ~DrawableBasePrivate();

    std::shared_ptr<graphics::IRenderProgram> &renderProgram();
    std::shared_ptr<graphics::IVertexArray> &vertexArray();
    std::unordered_map<int32_t, std::shared_ptr<AbstractUniform>> &uniforms();

protected:
    std::shared_ptr<graphics::IRenderProgram> m_renderProgram;
    std::shared_ptr<graphics::IVertexArray> m_vertexArray;
    std::unordered_map<int32_t, std::shared_ptr<AbstractUniform>> m_uniforms;

};

}
}

#endif // CORE_DRAWABLEBASEPRIVATE_H

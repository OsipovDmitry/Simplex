#ifndef CORE_DRAWABLEBASEPRIVATE_H
#define CORE_DRAWABLEBASEPRIVATE_H

#include <memory>
#include <string>
#include <unordered_map>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class DrawableBasePrivate
{
public:
    DrawableBasePrivate(std::shared_ptr<graphics::IVertexArray> vao);

    virtual ~DrawableBasePrivate();

    std::shared_ptr<graphics::IVertexArray> &vertexArray();
    std::unordered_map<graphics::UniformId, PAbstratcUniform> &uniforms();
    std::unordered_map<std::string, PAbstratcUniform> &userUniforms();
    std::unordered_map<graphics::SSBOId, std::shared_ptr<graphics::IBufferRange>> &SSBOs();

protected:
    std::shared_ptr<graphics::IVertexArray> m_vertexArray;
    std::unordered_map<graphics::UniformId, PAbstratcUniform> m_uniforms;
    std::unordered_map<std::string, PAbstratcUniform> m_userUniforms;
    std::unordered_map<graphics::SSBOId, std::shared_ptr<graphics::IBufferRange>> m_SSBOs;

};

}
}

#endif // CORE_DRAWABLEBASEPRIVATE_H

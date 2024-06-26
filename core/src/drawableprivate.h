#ifndef CORE_DRAWABLEPRIVATE_H
#define CORE_DRAWABLEPRIVATE_H

#include <memory>
#include <string>
#include <unordered_map>

#include <utils/boundingbox.h>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class DrawablePrivate
{
public:
    DrawablePrivate(const std::shared_ptr<graphics::IVertexArray>&);
    virtual ~DrawablePrivate();

    std::shared_ptr<graphics::IVertexArray> &vertexArray();
    std::unordered_map<graphics::UniformId, PAbstractUniform> &uniforms();
    std::unordered_map<std::string, PAbstractUniform> &userUniforms();
    std::unordered_map<graphics::SSBOId, std::shared_ptr<const graphics::IBufferRange>> &SSBOs();
    bool &isDoubleSided();

    static float &defaultAlphaCutoff();

protected:
    std::shared_ptr<graphics::IVertexArray> m_vertexArray;
    std::unordered_map<graphics::UniformId, PAbstractUniform> m_uniforms;
    std::unordered_map<std::string, PAbstractUniform> m_userUniforms;
    std::unordered_map<graphics::SSBOId, std::shared_ptr<const graphics::IBufferRange>> m_SSBOs;
    bool m_isDoubleSided;

    static float s_defaultAlphaCutoff;

};

}
}

#endif // CORE_DRAWABLEPRIVATE_H

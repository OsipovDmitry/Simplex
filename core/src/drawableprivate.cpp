#include "drawableprivate.h"

namespace simplex
{
namespace core
{

DrawablePrivate::DrawablePrivate(const std::shared_ptr<graphics::IVertexArray> &vao)
    : m_vertexArray(vao)
{}

DrawablePrivate::~DrawablePrivate() = default;

std::shared_ptr<graphics::IVertexArray> &DrawablePrivate::vertexArray()
{
    return m_vertexArray;
}

std::unordered_map<graphics::UniformId, PAbstractUniform> &DrawablePrivate::uniforms()
{
    return m_uniforms;
}

std::unordered_map<std::string, PAbstractUniform> &DrawablePrivate::userUniforms()
{
    return m_userUniforms;
}

std::unordered_map<graphics::SSBOId, std::shared_ptr<graphics::IBufferRange>> &DrawablePrivate::SSBOs()
{
    return m_SSBOs;
}



}
}

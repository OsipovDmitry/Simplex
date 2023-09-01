#include "drawablebaseprivate.h"

namespace simplex
{
namespace core
{

DrawableBasePrivate::DrawableBasePrivate(std::shared_ptr<graphics::IVertexArray> vao)
    : m_vertexArray(vao)
{}

DrawableBasePrivate::~DrawableBasePrivate()
{}

std::shared_ptr<graphics::IVertexArray> &DrawableBasePrivate::vertexArray()
{
    return m_vertexArray;
}

std::unordered_map<graphics::UniformId, PAbstratcUniform> &DrawableBasePrivate::uniforms()
{
    return m_uniforms;
}

std::unordered_map<std::string, PAbstratcUniform> &DrawableBasePrivate::userUniforms()
{
    return m_userUniforms;
}

std::unordered_map<graphics::SSBOId, std::shared_ptr<graphics::IBufferRange>> &DrawableBasePrivate::SSBOs()
{
    return m_SSBOs;
}



}
}

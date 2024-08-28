#include "drawableprivate.h"

namespace simplex
{
namespace core
{

DrawablePrivate::DrawablePrivate(const std::shared_ptr<graphics::IVertexArray> &vao)
    : StateSetPrivate()
    , m_vertexArray(vao)
{}

DrawablePrivate::~DrawablePrivate() = default;

std::shared_ptr<graphics::IVertexArray> &DrawablePrivate::vertexArray()
{
    return m_vertexArray;
}

bool &DrawablePrivate::isDoubleSided()
{
    return m_isDoubleSided;
}

}
}

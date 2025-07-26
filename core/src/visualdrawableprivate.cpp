#include "visualdrawableprivate.h"

namespace simplex
{
namespace core
{

VisualDrawablePrivate::VisualDrawablePrivate(const std::shared_ptr<graphics::IVertexArray> &vao, const utils::BoundingBox &bb)
    : DrawablePrivate(vao)
    , m_boundingBox(bb)
{
}

VisualDrawablePrivate::VisualDrawablePrivate(
    const std::shared_ptr<Mesh>& mesh,
    const std::shared_ptr<Material>& material,
    const utils::BoundingBox& bb)
    : DrawablePrivate(mesh, material)
    , m_boundingBox(bb)
{
}

VisualDrawablePrivate::~VisualDrawablePrivate() = default;

utils::BoundingBox &VisualDrawablePrivate::boundingBox()
{
    return m_boundingBox;
}

bool &VisualDrawablePrivate::isShadowCastedFlag()
{
    return m_isShadowCastedFlag;
}

}
}

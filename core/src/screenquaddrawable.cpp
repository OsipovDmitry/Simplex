#include <utils/mesh.h>
#include <utils/meshpainter.h>

#include <core/mesh.h>

#include "screenquaddrawable.h"

namespace simplex
{
namespace core
{

ScreenQuadDrawable::ScreenQuadDrawable()
    : Drawable(ScreenQuadDrawable::mesh(), nullptr)
{
}

ScreenQuadDrawable::~ScreenQuadDrawable() = default;

std::shared_ptr<const Mesh> ScreenQuadDrawable::mesh()
{
    static std::shared_ptr<Mesh> s_mesh;
    if (!s_mesh)
    {
        auto mesh = utils::Mesh::createEmptyMesh({
            {utils::VertexAttribute::Position, {2u, utils::VertexComponentType::Single}}
            });
        ;
        s_mesh = std::make_shared<Mesh>(utils::MeshPainter(mesh).drawScreenQuad().mesh());
    }

    return s_mesh;
}

}
}

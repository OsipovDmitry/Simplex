#include <utils/mesh.h>
#include <utils/meshpainter.h>

#include <core/mesh.h>

#include "screenquad.h"

namespace simplex
{
namespace core
{

ScreenQuad::~ScreenQuad() = default;

ScreenQuad::ScreenQuad()
    : Mesh()
{
    auto mesh = utils::Mesh::createEmptyMesh({
        {utils::VertexAttribute::Position, {2u, utils::VertexComponentType::Single}}
    });

    setMesh(utils::MeshPainter(mesh).drawScreenQuad().mesh(), utils::BoundingBox());
}

}
}

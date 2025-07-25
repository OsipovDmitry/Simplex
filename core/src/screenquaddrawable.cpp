#include <utils/mesh.h>
#include <utils/meshpainter.h>

#include <core/mesh.h>

#include "screenquaddrawable.h"

// tmp
#include <core/graphicsrendererbase.h>

namespace simplex
{
namespace core
{

ScreenQuadDrawable::~ScreenQuadDrawable() = default;

ScreenQuadDrawable::ScreenQuadDrawable()
    : Mesh(nullptr)
    /* tmp */, Drawable(graphics::RendererBase::current()->createVertexArray(
        utils::MeshPainter(utils::Mesh::createEmptyMesh({
            {utils::VertexAttribute::Position, {2u, utils::VertexComponentType::Single}}
            })).drawScreenQuad().mesh()))
{
    auto mesh = utils::Mesh::createEmptyMesh({
        {utils::VertexAttribute::Position, {2u, utils::VertexComponentType::Single}}
    });

    Mesh::setMesh(utils::MeshPainter(mesh).drawScreenQuad().mesh());
}

}
}

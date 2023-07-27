#include <utils/frustum.h>

#include <core/node.h>
#include <core/drawablenode.h>
#include <core/igraphicsrenderer.h>
#include <core/standarddrawable.h>

#include "drawablenoderenderer.h"

namespace simplex
{
namespace core
{

DrawableNodeRenderer::DrawableNodeRenderer(std::shared_ptr<graphics::IRenderer> renderer,
                                           const utils::Frustum &frustum,
                                           std::shared_ptr<StandardDrawable> nodeBoundingBoxDrawable,
                                           std::shared_ptr<StandardDrawable> drawableNodeLocalBoundingBoxDrawable)
    : FrustumCullingVisitor(frustum)
    , m_renderer(renderer)
    , m_nodeBoundingBoxDrawable(nodeBoundingBoxDrawable)
    , m_drawableNodeLocalBoundingBoxDrawable(drawableNodeLocalBoundingBoxDrawable)
{}

bool DrawableNodeRenderer::visit(std::shared_ptr<Node> node)
{
    if (!FrustumCullingVisitor::visit(node))
        return false;

    if (false)
    {
        const auto &nodeBoundingBox = node->boundingBox();
        m_renderer->addRenderData(node->globalTransform() *
                                  glm::translate(glm::mat4(1.f), nodeBoundingBox.center()) *
                                  glm::scale(glm::mat4(1.f), nodeBoundingBox.halfSize()),
                                  m_nodeBoundingBoxDrawable);
    }

    if (auto drawableNode = node->asDrawableNode(); drawableNode)
    {
        const auto &drawableNodeLocalBoundingBox = drawableNode->localBoundingBox();
        if (transformedFrustum().contain(drawableNodeLocalBoundingBox))
        {
            for (const auto &drawable : drawableNode->drawables())
                m_renderer->addRenderData(drawableNode->globalTransform(), drawable);

            if (false)
            {
                m_renderer->addRenderData(drawableNode->globalTransform() *
                                          glm::translate(glm::mat4(1.f), drawableNodeLocalBoundingBox.center()) *
                                          glm::scale(glm::mat4(1.f), drawableNodeLocalBoundingBox.halfSize()),
                                          m_drawableNodeLocalBoundingBoxDrawable);
            }
        }
    }

    return true;
}

}
}

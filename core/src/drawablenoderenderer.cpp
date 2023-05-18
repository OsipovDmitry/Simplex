#include <utils/frustum.h>

#include <core/node.h>
#include <core/drawablenode.h>
#include <core/coloreddrawable.h>

#include "drawablenoderenderer.h"
#include "nodeprivate.h"
#include "drawablenodeprivate.h"

namespace simplex
{
namespace core
{

DrawableNodeRenderer::DrawableNodeRenderer(std::shared_ptr<IGraphicsRenderer> renderer, const utils::Frustum &frustum)
    : FrustumCullingVisitor(frustum)
    , m_renderer(renderer)
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
                                  node->m().boundingBoxDrawable());
    }

    if (auto drawableNode = node->asDrawableNode(); drawableNode)
    {
        const auto &drawableNodeLocalBoundingBox = drawableNode->localBoundingBox();
        if (transformedFrustum().contain(drawableNodeLocalBoundingBox))
        {
            for (const auto &drawable : drawableNode->drawables())
                m_renderer->addRenderData(drawableNode->globalTransform(), drawable);

            if (true)
            {

                m_renderer->addRenderData(drawableNode->globalTransform() *
                                          glm::translate(glm::mat4(1.f), drawableNodeLocalBoundingBox.center()) *
                                          glm::scale(glm::mat4(1.f), drawableNodeLocalBoundingBox.halfSize()),
                                          drawableNode->m().localBoundingBoxDrawable());
            }
        }
    }

    return true;
}

}
}

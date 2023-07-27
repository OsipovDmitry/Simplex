#ifndef CORE_DRAWABLENODERENDERER_H
#define CORE_DRAWABLENODERENDERER_H

#include <core/forwarddecl.h>
#include <core/frustumcullingvisitor.h>

namespace simplex
{
namespace core
{

class DrawableNodeRenderer : public FrustumCullingVisitor
{
public:
    DrawableNodeRenderer(std::shared_ptr<graphics::IRenderer>,
                         const utils::Frustum&,
                         std::shared_ptr<StandardDrawable>,
                         std::shared_ptr<StandardDrawable>);

    bool visit(std::shared_ptr<Node>) override;

private:
    std::shared_ptr<graphics::IRenderer> m_renderer;
    std::shared_ptr<StandardDrawable> m_nodeBoundingBoxDrawable;
    std::shared_ptr<StandardDrawable> m_drawableNodeLocalBoundingBoxDrawable;
};

}
}

#endif // CORE_DRAWABLENODERENDERER_H

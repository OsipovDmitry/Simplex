#ifndef CORE_DRAWABLENODERENDERER_H
#define CORE_DRAWABLENODERENDERER_H

#include <core/igraphicsrenderer.h>
#include <core/frustumcullingvisitor.h>

namespace simplex
{
namespace core
{

class DrawableNodeRenderer : public FrustumCullingVisitor
{
public:
    DrawableNodeRenderer(std::shared_ptr<IGraphicsRenderer>, const utils::Frustum&);

    bool visit(std::shared_ptr<Node>) override;

private:
    std::shared_ptr<IGraphicsRenderer> m_renderer;
};

}
}

#endif // CORE_DRAWABLENODERENDERER_H

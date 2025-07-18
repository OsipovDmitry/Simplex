#ifndef CORE_VISUALDRAWABLEPRIVATE_H
#define CORE_VISUALDRAWABLEPRIVATE_H

#include "drawableprivate.h"

namespace simplex
{
namespace core
{

class VisualDrawablePrivate : public DrawablePrivate
{
public:
    VisualDrawablePrivate(const std::shared_ptr<graphics::IVertexArray>&, const utils::BoundingBox&);
    ~VisualDrawablePrivate() override;

    utils::BoundingBox &boundingBox();
    bool &isShadowCastedFlag();

private:
    utils::BoundingBox m_boundingBox;
    bool m_isShadowCastedFlag;
};

}
}

#endif // CORE_VISUALDRAWABLEPRIVATE_H

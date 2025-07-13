#ifndef CORE_POINTSHADOWPRIVATE_H
#define CORE_POINTSHADOWPRIVATE_H

#include "shadowprivate.h"

namespace simplex
{
namespace core
{

class PointShadowPrivate : public ShadowPrivate
{
public:
    PointShadowPrivate();
    ~PointShadowPrivate() override;

    std::shared_ptr<ShadowFrameBuffer> createShadowFrameBuffer(const std::shared_ptr<graphics::RendererBase>&) const override;

};

}
}

#endif // CORE_POINTSHADOWPRIVATE_H

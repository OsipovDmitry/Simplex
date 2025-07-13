#ifndef CORE_DIRECTIONALSHADOWPRIVATE_H
#define CORE_DIRECTIONALSHADOWPRIVATE_H

#include "shadowprivate.h"

namespace simplex
{
namespace core
{

class DirectionalShadowPrivate : public ShadowPrivate
{
public:
    DirectionalShadowPrivate();
    ~DirectionalShadowPrivate() override;

    std::shared_ptr<ShadowFrameBuffer> createShadowFrameBuffer(const std::shared_ptr<graphics::RendererBase>&) const override;

};

}
}

#endif // CORE_DIRECTIONALSHADOWPRIVATE_H

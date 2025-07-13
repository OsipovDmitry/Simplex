#ifndef CORE_SPOTSHADOWPRIVATE_H
#define CORE_SPOTSHADOWPRIVATE_H

#include "shadowprivate.h"

namespace simplex
{
namespace core
{

class SpotShadowPrivate : public ShadowPrivate
{
public:
    SpotShadowPrivate();
    ~SpotShadowPrivate() override;

    std::shared_ptr<ShadowFrameBuffer> createShadowFrameBuffer(const std::shared_ptr<graphics::RendererBase>&) const override;

};

}
}

#endif // CORE_SPOTSHADOWPRIVATE_H

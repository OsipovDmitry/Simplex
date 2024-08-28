#ifndef CORE_IBLSHADOWPRIVATE_H
#define CORE_IBLSHADOWPRIVATE_H

#include "shadowprivate.h"

namespace simplex
{
namespace core
{

class IBLShadowPrivate : public ShadowPrivate
{
public:
    IBLShadowPrivate();
    ~IBLShadowPrivate() override;

    std::shared_ptr<ShadowFrameBuffer> createShadowFrameBuffer(const std::shared_ptr<graphics::IRenderer>&) const override;

};

}
}

#endif // CORE_IBLSHADOWPRIVATE_H

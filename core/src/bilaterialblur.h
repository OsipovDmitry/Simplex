#ifndef CORE_BILATERIALBLUR_H
#define CORE_BILATERIALBLUR_H

#include "blur.h"

namespace simplex
{
namespace core
{

class BilaterialBlur : public Blur
{
public:
    BilaterialBlur();
    ~BilaterialBlur() override;

    graphics::PConstTexture sourceDepthMap() const;
    void setSourceDepthMap(const graphics::PConstTexture&);

    float maxDepthDifference() const;
    void setMaxDepthDifference(float);

private:
    static float s_defaultMaxDepthDifference;
};

}
}

#endif // CORE_BILATERIALBLUR_H

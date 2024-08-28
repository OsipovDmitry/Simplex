#ifndef CORE_SSAOPRIVATE_H
#define CORE_SSAOPRIVATE_H

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class SSAODrawable;
class BilaterialBlur;

class SSAOPrivate
{
public:
    SSAOPrivate();
    ~SSAOPrivate();

    SSAOMode &mode();
    float &viewportDenominator();
    std::shared_ptr<SSAODrawable> &drawable();
    std::shared_ptr<BilaterialBlur> &blur();

private:
    SSAOMode m_mode;
    float m_viewportDenominator;
    std::shared_ptr<SSAODrawable> m_drawable;
    std::shared_ptr<BilaterialBlur> m_blur;
};

}
}

#endif // CORE_SSAOPRIVATE_H

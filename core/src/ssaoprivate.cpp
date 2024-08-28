#include <core/settings.h>
#include <core/ssao.h>

#include "ssaoprivate.h"

namespace simplex
{
namespace core
{

SSAOPrivate::SSAOPrivate()
{
}

SSAOPrivate::~SSAOPrivate() = default;

SSAOMode &SSAOPrivate::mode()
{
    return m_mode;
}

float &SSAOPrivate::viewportDenominator()
{
    return m_viewportDenominator;
}

std::shared_ptr<SSAODrawable> &SSAOPrivate::drawable()
{
    return m_drawable;
}

std::shared_ptr<BilaterialBlur> &SSAOPrivate::blur()
{
    return m_blur;
}

}
}

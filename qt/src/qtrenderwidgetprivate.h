#ifndef QTRENDERWIDGETPRIVATE_H
#define QTRENDERWIDGETPRIVATE_H

#include <memory>

#include <core/forwarddecl.h>

namespace simplex
{
namespace qt
{

class QtOpenGL_1_0_Renderer;

class QtRenderWidgetPrivate
{
public:
    QtRenderWidgetPrivate() {}
    ~QtRenderWidgetPrivate() {}

    std::shared_ptr<QtOpenGL_1_0_Renderer> renderer;
    std::weak_ptr<core::IApplication> application;
};

}
}

#endif // QTRENDERWIDGETPRIVATE_H

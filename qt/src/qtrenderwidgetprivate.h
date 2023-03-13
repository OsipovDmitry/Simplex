#ifndef QTRENDERWIDGETPRIVATE_H
#define QTRENDERWIDGETPRIVATE_H

#include <memory>

#include <core/forwarddecl.h>

namespace simplex
{
namespace qt
{

class QtOpenGL_4_5_Renderer;

class QtRenderWidgetPrivate
{
public:
    QtRenderWidgetPrivate() {}
    ~QtRenderWidgetPrivate() {}

    std::shared_ptr<QtOpenGL_4_5_Renderer> renderer;
    std::weak_ptr<core::IApplication> application;
};

}
}

#endif // QTRENDERWIDGETPRIVATE_H

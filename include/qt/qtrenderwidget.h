#ifndef QTRENDERWIDGET_H
#define QTRENDERWIDGET_H

#include <QtOpenGLWidgets/QOpenGLWidget>

#include <utils/noncopyble.h>

#include <core/forwarddecl.h>

#include <qt/qtglobal.h>

namespace simplex
{
namespace qt
{

class QtRenderWidgetPrivate;
class QT_SHARED_EXPORT QtRenderWidget : public QOpenGLWidget
{
    NONCOPYBLE(QtRenderWidget)
public:
    QtRenderWidget();
    ~QtRenderWidget() override;

    void setApplication(std::shared_ptr<core::IApplication>);

    std::shared_ptr<core::IGraphicsRenderer> graphicsRenderer();
    std::shared_ptr<const core::IGraphicsRenderer> graphicsRenderer() const;

protected:
    void initializeGL() override;
    void resizeGL(int, int) override;
    void paintGL() override;

private:
    std::unique_ptr<QtRenderWidgetPrivate> m_;
};

}
}

#endif // QTRENDERWIDGET_H

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
    QtRenderWidget(QWidget* = nullptr);
    ~QtRenderWidget() override;

    void setApplication(std::weak_ptr<core::IApplication>);
    std::weak_ptr<core::IApplication> application();
    std::weak_ptr<const core::IApplication> application() const;

    std::shared_ptr<core::graphics::IRenderer> graphicsRenderer();
    std::shared_ptr<const core::graphics::IRenderer> graphicsRenderer() const;

protected:
    void resizeGL(int, int) override;
    void paintGL() override;

    void closeEvent(QCloseEvent*) override;

private:
    std::unique_ptr<QtRenderWidgetPrivate> m_;
};

}
}

#endif // QTRENDERWIDGET_H

#ifndef QT_QTOPENGLWIDGET_H
#define QT_QTOPENGLWIDGET_H

#include <QtOpenGLWidgets/QOpenGLWidget>

#include <utils/noncopyble.h>
#include <utils/pimpl.h>

#include <core/forwarddecl.h>
#include <core/irenderwidget.h>

#include <qt/qtglobal.h>

namespace simplex
{
namespace qt
{

class QtOpenGLWidgetPrivate;
class QT_SHARED_EXPORT QtOpenGLWidget : public QOpenGLWidget, public std::enable_shared_from_this<QtOpenGLWidget>, public core::IRenderWidget
{
    NONCOPYBLE(QtOpenGLWidget)
    PRIVATE_IMPL(QtOpenGLWidget)

public:
    ~QtOpenGLWidget() override;

    const std::string &name() const override;

    void setApplication(const std::shared_ptr<core::ApplicationBase>&);
    std::shared_ptr<core::ApplicationBase> application() const;

    std::shared_ptr<core::graphics::IRenderer> renderer();
    std::shared_ptr<const core::graphics::IRenderer> renderer() const;

    std::shared_ptr<core::graphics::IFrameBuffer> defaultFrameBuffer();
    std::shared_ptr<const core::graphics::IFrameBuffer> defaultFrameBuffer() const;

    static std::shared_ptr<QtOpenGLWidget> getOrCreate(const std::string&);

protected:
    void resizeGL(int, int) override;
    void paintGL() override;

private:
    QtOpenGLWidget(const std::string&);

    std::unique_ptr<QtOpenGLWidgetPrivate> m_;
};

}
}

#endif // QT_QTOPENGLWIDGET_H

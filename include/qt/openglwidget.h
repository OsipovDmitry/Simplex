#ifndef QT_OPENGLWIDGET_H
#define QT_OPENGLWIDGET_H

#include <QWidget>

#include <core/forwarddecl.h>
#include <core/irenderwidget.h>

#include <utils/noncopyble.h>
#include <utils/pimpl.h>

#include <qt/qtglobal.h>

class QSurface;

namespace simplex
{
namespace qt
{

class OpenGLWidgetPrivate;
class QT_SHARED_EXPORT OpenGLWidget : public QWidget, public std::enable_shared_from_this<OpenGLWidget>, public core::IRenderWidget
{
    NONCOPYBLE(OpenGLWidget)
    PRIVATE_IMPL(OpenGLWidget)

public:
    ~OpenGLWidget() override;

    const std::string &name() const override;

    QSurface *surface() const;

    void setApplication(const std::shared_ptr<core::ApplicationBase>&);
    std::shared_ptr<core::ApplicationBase> application() const;

    std::shared_ptr<core::graphics::IRenderer> renderer();
    std::shared_ptr<const core::graphics::IRenderer> renderer() const;

    std::shared_ptr<core::graphics::IFrameBuffer> defaultFrameBuffer();
    std::shared_ptr<const core::graphics::IFrameBuffer> defaultFrameBuffer() const;

    static std::shared_ptr<OpenGLWidget> getOrCreate(const std::string&);

protected:
    void resizeEvent(QResizeEvent*) override;
    void paintEvent(QPaintEvent*) override;

private:
    OpenGLWidget(const std::string&);

    std::unique_ptr<OpenGLWidgetPrivate> m_;
};

}
}

#endif // QT_OPENGLWIDGET_H

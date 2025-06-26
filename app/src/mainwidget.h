#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

#include <utils/glm/vec2.hpp>
#include <utils/glm/vec3.hpp>

#include <core/forwarddecl.h>

#include <qt/forwarddecl.h>

namespace Ui {
class MainWidget;
}

namespace simplex
{
namespace qt
{
class QtOpenGLWidget;
class OpenGLWidget;
}
}

class QListWidget;
class QComboBox;;

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget() override;

    QListWidget *renderTypeList();
    simplex::qt::RenderWidget *renderWidget();
    QComboBox *shadowModeBox();
    QComboBox *shadowFilterBox();

    void onTimeout();

    void setRenderMode(bool);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

signals:
    void renderTypeChanged(int);
    void shadowModeChanged(int);
    void shadowFilterChanged(int);

private:
    Ui::MainWidget *ui;
    //std::shared_ptr<simplex::qt::QtOpenGLWidget> m_renderWidget;
    std::shared_ptr<simplex::qt::RenderWidget> m_renderWidget;

    bool m_isWPressed = false;
    bool m_isSPressed = false;
    bool m_isAPressed = false;
    bool m_isDPressed = false;
    bool m_isUpPressed = false;
    bool m_isDownPressed = false;
    bool m_isLeftPressed = false;
    bool m_isRightPressed = false;

    glm::vec3 m_cameraPosition = glm::vec3(0.f, 1.f, 0.f);//glm::vec3(-0.17f, 1.17f, 3.0f);
    glm::vec2 m_cameraAngles = glm::vec2(0.f);//glm::vec2(-0.18f, 6.01f);
};

#endif // MAINWIDGET_H

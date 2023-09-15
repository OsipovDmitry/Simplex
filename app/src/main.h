#ifndef MAIN_H
#define MAIN_H

#include <QWidget>
#include <QListWidget>

#include <utils/glm/vec2.hpp>
#include <utils/glm/vec3.hpp>

namespace simplex
{
namespace qt
{
class QtRenderWidget;
}
}

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    MainWidget();

    QListWidget *renderTypeListWidget();
    simplex::qt::QtRenderWidget *renderWidget();

    void onTimeout();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

signals:
    void renderTypeChanged(int);

private:
    QListWidget *m_renderTypeListWidget;
    simplex::qt::QtRenderWidget *m_renderWidget;

    bool m_isWPressed = false;
    bool m_isSPressed = false;
    bool m_isAPressed = false;
    bool m_isDPressed = false;
    bool m_isUpPressed = false;
    bool m_isDownPressed = false;
    bool m_isLeftPressed = false;
    bool m_isRightPressed = false;

    glm::vec3 m_cameraPosition = glm::vec3(-0.17f, 1.17f, 3.0f);
    glm::vec2 m_cameraAngles = glm::vec2(-0.18f, 6.01f);

};

#endif // MAIN_H

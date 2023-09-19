#include <QApplication>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QSplitter>
#include <QPointer>
#include <QTimer>
#include <QKeyEvent>

#include <iostream>

#include <utils/logger.h>
#include <utils/shader.h>
#include <utils/transform.h>
#include <utils/glm/gtc/constants.hpp>

#include <core/graphicsengine.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/cameranode.h>
#include <core/nodevisitor.h>
#include <core/nodecollector.h>

#include <qt/qtrenderwidget.h>

#include "main.h"
#include "testapplication.h"

class LoggerOutput : public simplex::utils::Logger::Output
{
public:
    LoggerOutput(QTextEdit *textEdit) : simplex::utils::Logger::Output(), m_textEdit(textEdit) {}
    void operator <<(const std::string &s) override {
        if (m_textEdit)
        {
            m_textEdit->insertPlainText(QString::fromStdString(s) + "\n");

//            auto cursor = m_textEdit->textCursor();
//            cursor.setPosition(QTextCursor::End);
//            m_textEdit->setTextCursor(cursor);
        }
    }

private:
    QPointer<QTextEdit> m_textEdit;
};

MainWidget::MainWidget()
    : QWidget(nullptr)
    , m_renderTypeListWidget(new QListWidget())
    , m_renderWidget(new simplex::qt::QtRenderWidget())
{
    setWindowTitle("Simplex 3D Engine");

    m_renderTypeListWidget->addItem("Base color");
    m_renderTypeListWidget->addItem("Metalness");
    m_renderTypeListWidget->addItem("Roughness");
    m_renderTypeListWidget->addItem("Normals");
    m_renderTypeListWidget->addItem("Depth");
    m_renderTypeListWidget->addItem("OIT indices");
    m_renderTypeListWidget->addItem("OIT fragments count");
    m_renderTypeListWidget->addItem("Final color");
    m_renderTypeListWidget->addItem("Final metalness");
    m_renderTypeListWidget->addItem("Final roughness");
    m_renderTypeListWidget->addItem("Final normals");
    m_renderTypeListWidget->addItem("Final depth");
    QObject::connect(m_renderTypeListWidget, &QListWidget::currentRowChanged, this, &MainWidget::renderTypeChanged);

    auto splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(m_renderWidget);
    splitter->addWidget(m_renderTypeListWidget);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);

    auto layout = new QVBoxLayout();
    layout->addWidget(splitter);
    setLayout(layout);

    auto timer = new QTimer(this);
    timer->setInterval(16);
    QObject::connect(timer, &QTimer::timeout, this, &MainWidget::onTimeout);
    timer->start();
}

QListWidget *MainWidget::renderTypeListWidget()
{
    return m_renderTypeListWidget;
}

simplex::qt::QtRenderWidget *MainWidget::renderWidget()
{
    return m_renderWidget;
}

void MainWidget::onTimeout()
{
    m_renderWidget->setFocus();

    if (m_renderWidget->application().expired())
        return;

    if (m_isUpPressed)
        m_cameraAngles.x += 0.03f;

    if (m_isDownPressed)
        m_cameraAngles.x -= 0.03f;

    if (m_isLeftPressed)
        m_cameraAngles.y += 0.03f;

    if (m_isRightPressed)
        m_cameraAngles.y -= 0.03f;

    m_cameraAngles.x = glm::max(m_cameraAngles.x, -glm::half_pi<float>());
    m_cameraAngles.x = glm::min(m_cameraAngles.x, +glm::half_pi<float>());
    m_cameraAngles.y = glm::mod(m_cameraAngles.y, glm::two_pi<float>());

    auto cameraRotation = simplex::utils::Transform::fromRotation(glm::quat(glm::vec3(m_cameraAngles, 0.0f)));
    auto cameraFowrardDir = cameraRotation * glm::vec3(0.0f, 0.0f, -1.0f);
    auto cameraRightDir = cameraRotation * glm::vec3(1.0f, 0.0f, 0.0f);

    if (m_isWPressed)
        m_cameraPosition += cameraFowrardDir * 0.1f;

    if (m_isSPressed)
        m_cameraPosition -= cameraFowrardDir * 0.1f;

    if (m_isDPressed)
        m_cameraPosition += cameraRightDir * 0.1f;

    if (m_isAPressed)
        m_cameraPosition -= cameraRightDir * 0.1f;

    auto cameraTranslation = simplex::utils::Transform::fromTranslation(m_cameraPosition);
    auto cameraTransform = cameraTranslation * cameraRotation;

    auto app = m_renderWidget->application().lock();
    if (auto testApplication = std::dynamic_pointer_cast<TestApplication>(app); testApplication)
    {
        const auto &scenes = testApplication->graphicsEngine()->scenes();

        simplex::core::NodeCollector<simplex::core::CameraNode> cameraCollector;
        scenes.front()->sceneRootNode()->acceptDown(cameraCollector);

        cameraCollector.nodes().front()->setTransform(cameraTransform);
    }

    m_renderWidget->update();
}

void MainWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key::Key_W:
        m_isWPressed = true;
        break;
    case Qt::Key::Key_S:
        m_isSPressed = true;
        break;
    case Qt::Key::Key_A:
        m_isAPressed = true;
        break;
    case Qt::Key::Key_D:
        m_isDPressed = true;
        break;
    case Qt::Key::Key_Up:
        m_isUpPressed = true;
        break;
    case Qt::Key::Key_Down:
        m_isDownPressed = true;
        break;
    case Qt::Key::Key_Left:
        m_isLeftPressed = true;
        break;
    case Qt::Key::Key_Right:
        m_isRightPressed = true;
        break;
    case Qt::Key::Key_L: {
        const auto &scenes = std::dynamic_pointer_cast<TestApplication>(m_renderWidget->application().lock())->graphicsEngine()->scenes();

        simplex::core::NodeCollector<simplex::core::CameraNode> cameraCollector;
        scenes.front()->sceneRootNode()->acceptDown(cameraCollector);

        auto transform = cameraCollector.nodes().front()->transform();
        LOG_INFO << transform.translation.x << ", " << transform.translation.y << ", " << transform.translation.z;
        LOG_INFO << transform.rotation.w << ", " << transform.rotation.x << ", " << transform.rotation.y << ", " << transform.rotation.z;

        break; }
    default:
        break;
    }

    event->accept();
}

void MainWidget::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key::Key_W:
        m_isWPressed = false;
        break;
    case Qt::Key::Key_S:
        m_isSPressed = false;
        break;
    case Qt::Key::Key_A:
        m_isAPressed = false;
        break;
    case Qt::Key::Key_D:
        m_isDPressed = false;
        break;
    case Qt::Key::Key_Up:
        m_isUpPressed = false;
        break;
    case Qt::Key::Key_Down:
        m_isDownPressed = false;
        break;
    case Qt::Key::Key_Left:
        m_isLeftPressed = false;
        break;
    case Qt::Key::Key_Right:
        m_isRightPressed = false;
        break;
    default:
        break;
    }

    event->accept();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auto mainWidget = new MainWidget();
    mainWidget->showMaximized();

    auto testApplication = std::make_shared<TestApplication>(mainWidget->renderWidget()->graphicsRenderer());
    mainWidget->renderWidget()->setApplication(testApplication);

    QObject::connect(mainWidget, &MainWidget::renderTypeChanged, [testApplication](int row) {
        testApplication->graphicsEngine()->setF(row);
    });
    mainWidget->renderTypeListWidget()->setCurrentRow(7);

    auto result = QApplication::exec();

    delete mainWidget;
    return result;
}

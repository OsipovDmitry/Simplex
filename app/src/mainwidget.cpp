#include <QTimer>
#include <QKeyEvent>

#include <utils/logger.h>
#include <utils/transform.h>
#include <utils/glm/gtc/constants.hpp>

#include <core/applicationbase.h>
#include <core/graphicsengine.h>
#include <core/nodecollector.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/cameranode.h>
#include <core/soundnode.h>
#include <core/debuginformation.h>

#include <qt/qtopenglwidget.h>

#include "mainwidget.h"
#include "ui_mainwidget.h"

#include <utils/frustum.h>
#include <core/lightnode.h>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    m_renderWidget = simplex::qt::QtOpenGLWidget::getOrCreate("OpenGLWidget");
    ui->splitter_2->insertWidget(0, m_renderWidget.get());

    ui->splitter_3->setStretchFactor(0, 3);
    ui->splitter_3->setStretchFactor(1, 1);
    ui->splitter_2->setStretchFactor(0, 3);
    ui->splitter_2->setStretchFactor(1, 1);

    setWindowTitle("Simplex 3D Engine");

    QObject::connect(ui->m_renderTypeList, &QListWidget::currentRowChanged, this, &MainWidget::renderTypeChanged);
    QObject::connect(ui->m_shadowModeBox, &QComboBox::currentIndexChanged, this, &MainWidget::shadowModeChanged);
    QObject::connect(ui->m_shadowFilterBox, &QComboBox::currentIndexChanged, this, &MainWidget::shadowFilterChanged);

    auto timer = new QTimer(this);
    timer->setInterval(16);
    QObject::connect(timer, &QTimer::timeout, this, &MainWidget::onTimeout);
    timer->start();

    setRenderMode(true);
}

MainWidget::~MainWidget()
{
    delete ui;
    m_renderWidget = nullptr;
}

QListWidget *MainWidget::renderTypeList()
{
    return ui->m_renderTypeList;
}

simplex::qt::QtOpenGLWidget *MainWidget::renderWidget()
{
    return m_renderWidget.get();
}

QComboBox *MainWidget::shadowModeBox()
{
    return ui->m_shadowModeBox;
}

QComboBox *MainWidget::shadowFilterBox()
{
    return ui->m_shadowFilterBox;
}

void MainWidget::onTimeout()
{
    setFocus();

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

    auto cameraRotation = simplex::utils::Transform::makeRotation(glm::quat(glm::vec3(m_cameraAngles, 0.0f)));
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

    auto cameraTranslation = simplex::utils::Transform::makeTranslation(m_cameraPosition);
    auto cameraTransform = cameraTranslation * cameraRotation;

    if (auto application = m_renderWidget->application())
    {
        const auto &scenes = application->scenes();
        if (!scenes.empty())
        {
            simplex::core::NodeCollector<simplex::core::CameraNode> cameraCollector;
            (*scenes.begin())->sceneRootNode()->acceptDown(cameraCollector);

            cameraCollector.nodes().front()->setTransform(cameraTransform);
        }
    }

    m_renderWidget->update();
}

void MainWidget::setRenderMode(bool state)
{
    ui->m_loggerTextEdit->setVisible(state);
    ui->m_lightInfoWidget->setVisible(state);
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
    case Qt::Key::Key_Space:
    {
        auto application = m_renderWidget->application();
        auto graphicsEngine = application->findEngine<simplex::core::GraphicsEngine>();
        graphicsEngine->setF(-1);

        break;
    }
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


#include <QApplication>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QSplitter>
#include <QPointer>
#include <QListWidget>
#include <QTimer>

#include <iostream>

#include <utils/logger.h>
#include <utils/shader.h>

#include <core/graphicsengine.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/cameranode.h>
#include <core/nodevisitor.h>
#include <core/collectorvisitor.h>

#include <qt/qtrenderwidget.h>

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

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    auto textEdit = new QTextEdit;
//    textEdit->setReadOnly(true);
//    simplex::utils::Logger::setOutput(std::make_shared<LoggerOutput>(textEdit));


    auto listWidget = new QListWidget;
    listWidget->addItem("Base color");
    listWidget->addItem("Metalness");
    listWidget->addItem("Roughness");
    listWidget->addItem("Normals");
    listWidget->addItem("Depth");

    auto renderWidget = new simplex::qt::QtRenderWidget;

    auto splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(renderWidget);
    splitter->addWidget(listWidget);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);

//    auto widget = new QSplitter(Qt::Vertical);
//    widget->addWidget(splitter);
//    widget->addWidget(textEdit);
//    widget->setStretchFactor(0, 3);
//    widget->setStretchFactor(1, 1);

    splitter->setWindowTitle("Simplex 3D Engine");
    splitter->showMaximized();

    auto testApplication = std::make_shared<TestApplication>(renderWidget->graphicsRenderer());
    renderWidget->setApplication(testApplication);

//    auto sceneRootNode = testApplication->graphicsEngine()->scenes().begin()->get()->sceneRootNode();

//    simplex::core::CollectorVisitor<simplex::core::CameraNode> cameraNodeCollector;
//    sceneRootNode->accept(cameraNodeCollector);

//    for (auto &camera : cameraNodeCollector.nodes())
//    {
//        listWidget->addItem(QString::fromStdString(camera->name()));
//    }
    QObject::connect(listWidget, &QListWidget::currentRowChanged, [renderWidget, testApplication](int row) {
        testApplication->graphicsEngine()->setF(row);
        renderWidget->update();
    });
    listWidget->setCurrentRow(0);

    auto timer = new QTimer(renderWidget);
    timer->setInterval(16);
    QObject::connect(timer, &QTimer::timeout, [renderWidget](){ renderWidget->update(); });
    timer->start();

    auto result = QApplication::exec();

    delete timer;
    delete splitter;
    return result;
}

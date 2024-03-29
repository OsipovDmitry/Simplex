#include <QApplication>
#include <QPointer>
#include <QListWidget>
#include <QTextEdit>

#include <utils/logger.h>

#include <core/graphicsengine.h>

#include <qt/qtrenderwidget.h>

#include "mainwidget.h"
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

    auto mainWidget = new MainWidget();
    mainWidget->setRenderMode(false);
    auto renderWidget = mainWidget->renderWidget();

    mainWidget->showMaximized();

    auto testApplication = std::make_shared<TestApplication>(renderWidget->graphicsRenderer());
    renderWidget->setApplication(testApplication);

    QObject::connect(mainWidget, &MainWidget::renderTypeChanged, [testApplication](int row) {
        testApplication->graphicsEngine()->setF(row);
    });
    mainWidget->renderTypeList()->setCurrentRow(7);

    auto result = QApplication::exec();

    delete mainWidget;

    return result;
}

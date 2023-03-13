#include <QApplication>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QSplitter>

#include <iostream>

#include <utils/logger.h>

#include <qt/qtrenderwidget.h>

#include "testapplication.h"

class LoggerOutput : public simplex::utils::Logger::Output
{
public:
    LoggerOutput(QTextEdit *textEdit) : simplex::utils::Logger::Output(), m_textEdit(textEdit) {}
    void operator <<(const std::string& s) override { m_textEdit->insertPlainText(QString::fromStdString(s) + "\n"); }

private:
    QTextEdit *m_textEdit;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auto textEdit = new QTextEdit;
    textEdit->setReadOnly(true);
    simplex::utils::Logger::setOutput(std::make_shared<LoggerOutput>(textEdit));

    auto renderWidget = new simplex::qt::QtRenderWidget;

    QSplitter widget(Qt::Vertical);
    widget.addWidget(renderWidget);
    widget.addWidget(textEdit);
    widget.setWindowTitle("Simplex 3D Engine");
    widget.resize(800, 600);
    widget.show();

    auto graphicsRenderer = renderWidget->graphicsRenderer();

    auto testApplication = std::make_shared<TestApplication>(graphicsRenderer);
    renderWidget->setApplication(testApplication);

    return QApplication::exec();
}

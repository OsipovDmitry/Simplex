#include <QApplication>

#include <iostream>

#include <qt/qtrenderwidget.h>

#include "testapplication.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auto renderWidget = std::make_shared<simplex::qt::QtRenderWidget>();
    renderWidget->show();

    auto graphicsRenderer = renderWidget->graphicsRenderer();

    auto testApplication = std::make_shared<TestApplication>(graphicsRenderer);
    renderWidget->setApplication(testApplication);

    auto res = QApplication::exec();
    std::cout << "finish" << std::endl;
    return res;
}

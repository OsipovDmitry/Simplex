#include <QApplication>
#include <QComboBox>

#include <core/shadow.h>

#include <qt/qtopenglwidget.h>
#include <openal/openaldevice.h>

#include "mainwidget.h"
#include "testapplication.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auto openALDevice = simplex::openal::OpenALDevice::getOrCreate();

    auto mainWidget = new MainWidget();
    mainWidget->setRenderMode(true);
    mainWidget->showMaximized();

    auto testApplication = std::make_shared<TestApplication>(mainWidget->renderWidget(), openALDevice);
    mainWidget->renderWidget()->setApplication(testApplication);

    QObject::connect(mainWidget, &MainWidget::shadowModeChanged, [testApplication](int value) {testApplication->setShadowMode(simplex::core::castToShadingMode(value));});
    QObject::connect(mainWidget, &MainWidget::shadowFilterChanged, [testApplication](int value) {testApplication->setShadowFilter(simplex::core::castToShadingFilter(value));});
    testApplication->setShadowMode(simplex::core::castToShadingMode(mainWidget->shadowModeBox()->currentIndex()));
    testApplication->setShadowFilter(simplex::core::castToShadingFilter(mainWidget->shadowFilterBox()->currentIndex()));

    auto result = QApplication::exec();
    mainWidget->disconnect();

    testApplication.reset();
    delete mainWidget;

    return result;
}

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

#include <utils/image.h>
#include <utils/cubemap.h>
#include <utils/iblcalculator.h>

int main(int argc, char *argv[])
{
    //simplex::utils::IBLCalculator::calclulateBRDFLut()->saveToFile("D:/c/brdf.png");

    auto src = simplex::utils::Image::loadFromFile("D:/c/latlong.hdr");

    //simplex::utils::IBLCalculator::calculateIrradiance(src)->saveToFile("D:/c/irradiance.hdr");

//    auto radiances = simplex::utils::IBLCalculator::calculateRadiance(src);
//    for (size_t i = 0u; i < radiances.size(); ++i)
//        radiances[i]->saveToFile("D:/c/radiance"+std::to_string(i)+".hdr");

    //simplex::utils::CubemapConverter::convertSphericalCubemapToHCross(src)->saveToFile("D:/c/hcross.hdr");
    //simplex::utils::CubemapConverter::convertSphericalCubemapToVCross(src)->saveToFile("D:/c/vcross.hdr");

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

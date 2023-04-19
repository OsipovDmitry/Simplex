#include <QApplication>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QSplitter>
#include <QPointer>
#include <QListWidget>

#include <iostream>

#include <utils/logger.h>

#include <core/graphicsengine.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/cameranode.h>
#include <core/nodecollector.h>

#include <qt/qtrenderwidget.h>

#include "testapplication.h"

class LoggerOutput : public simplex::utils::Logger::Output
{
public:
    LoggerOutput(QTextEdit *textEdit) : simplex::utils::Logger::Output(), m_textEdit(textEdit) {}
    void operator <<(const std::string& s) override {
        if (m_textEdit)
            m_textEdit->insertPlainText(QString::fromStdString(s) + "\n");
    }

private:
    QPointer<QTextEdit> m_textEdit;
};

#include <fstream>
#include <utils/glm/glm.hpp>

int main(int argc, char *argv[])
{
    std::vector<glm::vec4> vertices;
    //std::vector<glm::vec4> normals;
    //std::vector<glm::vec4> tcs;
    std::vector<uint32_t> indices;

    std::ifstream v("D:/v.ply");
    while (!v.eof())
    {
        float x,y,z, nx,ny,nz, tcx,tcy;
        v >> x >>y >> z >> nx >> ny >> nz >> tcx >> tcy;
        vertices.push_back(glm::vec4(x,y,z,1.0f));
        //normals.push_back(glm::vec4(nx, ny, nz, 0.0f));
        //tcs.push_back(glm::vec4(tcx, tcy, 0.0f, 1.0f));
    }
    v.close();

    std::ifstream i("D:/i.ply");
    while (!i.eof())
    {
        uint32_t n, i0, i1, i2, i3;
        i >> n >> i0 >> i1 >> i2;
        i >> i3;
//        indices.push_back(i0);
//        indices.push_back(i1);
//        indices.push_back(i2);
        indices.push_back(i0); indices.push_back(i1);
        indices.push_back(i1); indices.push_back(i2);
        indices.push_back(i2); indices.push_back(i3);
        indices.push_back(i3); indices.push_back(i0);
    }
    i.close();

    const std::string primName = "camera";

    std::ofstream f("D:/output.txt");
    f << std::fixed;
    f << "#include <vector>" << std::endl;
    f << std::endl;
    f << "#include <utils/glm/vec4.hpp>" << std::endl;
    f << std::endl;
    f << "namespace simplex" << std::endl;
    f << "{" << std::endl;
    f << "namespace utils" << std::endl;
    f << "{" << std::endl;
    f << std::endl;

    f << "static const std::vector<glm::vec4> s_" << primName << "Vertices {" << std::endl;
    for (auto & v : vertices)
        f << "    {" << v.x << "f, " << v.y << "f, " << v.z << "f, " << v.w << "f}," << std::endl;
    f << "};" << std::endl << std::endl;

//    f << "static const std::vector<glm::vec4> s_" << primName << "Normals {" << std::endl;
//    for (auto & v : normals)
//        f << "    {" << v.x << "f, " << v.y << "f, " << v.z << "f, " << v.w << "f}," << std::endl;
//    f << "};" << std::endl << std::endl;

//    f << "static const std::vector<glm::vec4> s_" << primName << "TexCoords {" << std::endl;
//    for (auto & v : tcs)
//        f << "    {" << v.x << "f, " << v.y << "f, " << v.z << "f, " << v.w << "f}," << std::endl;
//    f << "};" << std::endl << std::endl;

    f << "static const std::vector<uint32_t> s_" << primName << "Indices {" << std::endl;
    for (size_t i = 0; i < indices.size() / 2; ++i)
    {
        f << "    ";
        for (size_t j = 0; j < 2; ++j)
            f << indices[i*2+j] << "u, ";
        f << std::endl;
    }
    f << "};" << std::endl << std::endl;

    f << "}" << std::endl;
    f << "}" << std::endl;
    f.close();

    QApplication a(argc, argv);

    auto textEdit = new QTextEdit;
    textEdit->setReadOnly(true);
    simplex::utils::Logger::setOutput(std::make_shared<LoggerOutput>(textEdit));

    auto renderWidget = new simplex::qt::QtRenderWidget;
    auto listWidget = new QListWidget;

    auto splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(renderWidget);
    splitter->addWidget(listWidget);

    auto widget = new QSplitter(Qt::Vertical);
    widget->addWidget(splitter);
    widget->addWidget(textEdit);
    widget->setWindowTitle("Simplex 3D Engine");
    widget->resize(800, 600);
    widget->show();

    auto testApplication = std::make_shared<TestApplication>(renderWidget->graphicsRenderer());
    renderWidget->setApplication(testApplication);

    auto cameraNodeCollector = std::make_shared<simplex::core::NodeCollector<simplex::core::CameraNode>>();
    auto sceneRootNode = testApplication->graphicsEngine()->scenes().begin()->get()->sceneRootNode();
    sceneRootNode->accept(cameraNodeCollector);

    for (auto &camera : cameraNodeCollector->nodes())
    {
        listWidget->addItem(QString::fromStdString(camera->name()));
    }
    QObject::connect(listWidget, &QListWidget::currentRowChanged, [renderWidget, cameraNodeCollector](int row) {
        for (auto &camera : cameraNodeCollector->nodes())
            camera->setRenderingEnabled(false);
        cameraNodeCollector->nodes()[row]->setRenderingEnabled(true);
        renderWidget->update();
    });
    listWidget->setCurrentRow(0);

    return QApplication::exec();
}

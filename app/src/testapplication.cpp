#include <utils/logger.h>
#include <utils/boundingbox.h>
#include <utils/clipspace.h>
#include <utils/frustum.h>
#include <utils/mesh.h>
#include <utils/meshpainter.h>
#include <utils/glm/gtc/matrix_transform.hpp>

#include <core/graphicsengine.h>
#include <core/audioengine.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/cameranode.h>
#include <core/pointlightnode.h>
#include <core/spotlightnode.h>
#include <core/directionallightnode.h>
#include <core/ibllightnode.h>
#include <core/shadow.h>
#include <core/visualdrawablenode.h>
#include <core/soundnode.h>
#include <core/listenernode.h>
#include <core/igraphicsrenderer.h>
#include <core/iaudiorenderer.h>
#include <core/pbrdrawable.h>
#include <core/texturesmanager.h>
#include <core/soundsmanager.h>

#include <qt/qtopenglwidget.h>
#include <qt/openglwidget.h>
#include <openal/openaldevice.h>

#include "testapplication.h"

float random(float a = 0.0f, float b = 1.0f)
{
    auto r = static_cast<float>(rand()) / (RAND_MAX-1.0f);
    return a + r * (b-a);
}

using namespace simplex;

const std::string ApplicationName = "Simplex3D";
const std::string GraphicsEngineName = "Simplex3DGraphicsEngine";
const std::string AudioEngineName = "Simplex3DAudioEngine";
const std::string SceneName = "Scene0";

TestApplication::TestApplication(qt::RenderWidget *openGLWidget,
                                 const std::shared_ptr<simplex::openal::OpenALDevice> &openALDevice)
    : simplex::core::ApplicationBase(ApplicationName)
    , m_renderWidget(openGLWidget)
    , m_audioDevice(openALDevice)
{
    auto graphicsRenderer = m_renderWidget->renderer();
    auto graphicsEngineInstance = std::make_shared<simplex::core::GraphicsEngine>(GraphicsEngineName, graphicsRenderer);
    registerEngine(graphicsEngineInstance);

    auto audioRenderer = m_audioDevice->renderer();
    auto audioEngineInstance = std::make_shared<simplex::core::AudioEngine>(AudioEngineName, audioRenderer);
    registerEngine(audioEngineInstance);
}

TestApplication::~TestApplication()
{
    m_renderWidget->renderer()->makeCurrent(m_renderWidget->shared_from_this());
    m_audioDevice->renderer()->makeCurrent();

    shutDown();

    m_renderWidget->renderer()->doneCurrent(m_renderWidget->shared_from_this());
    m_audioDevice->renderer()->doneCurrent();
}

void TestApplication::doInitialize()
{
    prepareRiggedFigureScene();
}

void TestApplication::doUpdate(uint64_t time, uint32_t dt)
{

}

void TestApplication::prepareStandardScene()
{
    auto graphicsEngineInstance = findEngine<core::GraphicsEngine>();
    auto audioEngineInstance = findEngine<core::AudioEngine>();

    auto texturesManager = graphicsEngineInstance->texturesManager();
    auto soundsManager = audioEngineInstance->soundsManager();

    auto graphicsRenderer = graphicsEngineInstance->graphicsRenderer();
    auto audioRenderer = audioEngineInstance->audioRenderer();

    utils::MeshPainter planePainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                  {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                  {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}},
                                                                  {utils::VertexAttribute::Tangent, {4u, utils::VertexComponentType::Single}}
                                                                 }));
    planePainter.setVertexTransform(utils::Transform(10.f, glm::quat(glm::vec3(-glm::half_pi<float>(), 0.f, 0.f))));
    planePainter.setTexCoordsTransform(utils::Transform::makeScale(4.f));
    planePainter.drawPlane();
    auto planeDrawable = std::make_shared<core::PBRDrawable>(graphicsRenderer->createVertexArray(planePainter.mesh()), planePainter.calculateBoundingBox());
    planeDrawable->setBaseColorMap(texturesManager->loadOrGetTexture("D:/res/textures/brick/bc.jpg"));
    planeDrawable->setNormalMap(texturesManager->loadOrGetTexture("D:/res/textures/brick/n.jpg"));
    planeDrawable->setMetalness(0.0f);
    planeDrawable->setRoughness(0.6f);

    utils::MeshPainter teapotPainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                   {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                   {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}}
                                                                }));
    teapotPainter.drawTeapot();
    auto teapotBoundingBox = teapotPainter.calculateBoundingBox();
    auto teapotVao = graphicsRenderer->createVertexArray(teapotPainter.mesh());

    std::array<std::array<std::shared_ptr<core::PBRDrawable>, 1u>, 1u> teapotDrawables;
    for (uint32_t r = 0u; r < teapotDrawables.size(); ++r)
        for (uint32_t m = 0u; m < teapotDrawables[r].size(); ++m)
        {
            teapotDrawables[r][m] = std::make_shared<core::PBRDrawable>(teapotVao, teapotBoundingBox);
            teapotDrawables[r][m]->setBaseColor(glm::vec4(1.f, 1.f, 1.f, 1.f));
            teapotDrawables[r][m]->setMetalness(static_cast<float>(m) / static_cast<float>(teapotDrawables[r].size()-1u));
            teapotDrawables[r][m]->setRoughness(static_cast<float>(r) / static_cast<float>(teapotDrawables.size()-1u));
        }

    utils::MeshPainter boxPainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}}
                                                                }));
    boxPainter.drawCube(glm::vec3(0.6f, 1.0f, 0.1f));
    auto boxDrawable = std::make_shared<core::PBRDrawable>(graphicsRenderer->createVertexArray(boxPainter.mesh()), boxPainter.calculateBoundingBox());
    boxDrawable->setBaseColor(glm::vec4(glm::vec3(1.f), 0.5f));
    boxDrawable->setBaseColorMap(texturesManager->loadOrGetTexture("D:/res/textures/glass1.jpg"));

    utils::MeshPainter tetraPainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                  {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                  {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}}
                                                                }));
    tetraPainter.setVertexTransform(utils::Transform::makeScale(1.8f));
    tetraPainter.drawTetrahedron();
    auto tetraDrawable = std::make_shared<core::PBRDrawable>(graphicsRenderer->createVertexArray(tetraPainter.mesh()), tetraPainter.calculateBoundingBox());
    tetraDrawable->setBaseColor(glm::vec4(glm::vec3(1.f), 0.5f));
    tetraDrawable->setBaseColorMap(texturesManager->loadOrGetTexture("D:/res/textures/glass2.jpg"));

    utils::MeshPainter spherePainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                   {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                   {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}}
                                                                }));
    spherePainter.setVertexTransform(utils::Transform::makeScale(.4f));
    spherePainter.drawSphere(16);
    auto sphereDrawable = std::make_shared<core::PBRDrawable>(graphicsRenderer->createVertexArray(spherePainter.mesh()), spherePainter.calculateBoundingBox());
    sphereDrawable->setBaseColor(glm::vec4(glm::vec3(1.f), 0.5f));
    sphereDrawable->setBaseColorMap(texturesManager->loadOrGetTexture("D:/res/textures/glass3.jpg"));

    utils::MeshPainter suzannePainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                   {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                   {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}}
                                                                }));
    suzannePainter.setVertexTransform(utils::Transform::makeScale(0.8f));
    suzannePainter.drawSuzanne();
    auto suzanneDrawable = std::make_shared<core::PBRDrawable>(graphicsRenderer->createVertexArray(suzannePainter.mesh()), suzannePainter.calculateBoundingBox());
    suzanneDrawable->setBaseColor(glm::vec4(glm::vec3(1.f), 0.5f));
    suzanneDrawable->setBaseColorMap(texturesManager->loadOrGetTexture("D:/res/textures/glass4.jpg"));

    utils::MeshPainter bunnyPainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                  {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                  {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}}
                                                                }));
    bunnyPainter.setVertexTransform(utils::Transform::makeScale(0.8f));
    bunnyPainter.drawBunny();
    auto bunnyDrawable = std::make_shared<core::PBRDrawable>(graphicsRenderer->createVertexArray(bunnyPainter.mesh()), bunnyPainter.calculateBoundingBox());
    bunnyDrawable->setBaseColor(glm::vec4(glm::vec3(255.f/255.f, 223.f/255.f, 40.f/255.f), .5f));
    bunnyDrawable->setMetalness(.5f);
    bunnyDrawable->setRoughness(0.f);

    auto scene0 = loadEmptyScene(SceneName);

    auto cameraNode0 = std::make_shared<core::CameraNode>("", m_renderWidget->defaultFrameBuffer());
    cameraNode0->setTransform(utils::Transform::makeTranslation(5.0f * glm::vec3(2.5f, 0.5f, 2.5f)) *
                              utils::Transform::makeRotation(glm::quat(glm::vec3(-glm::pi<float>()/9.f, glm::quarter_pi<float>(), 0.f))));
    scene0->sceneRootNode()->attach(cameraNode0);

    auto pointLight = std::make_shared<core::PointLightNode>("Point light");
    pointLight->setColor(glm::vec3(5.f));
    pointLight->setTransform(utils::Transform::makeTranslation(glm::vec3(0.f, 1.8f, 0.f)));
    pointLight->setRadiuses(glm::vec2(3.f, 5.0f));
    //scene0->sceneRootNode()->attach(pointLight);

    auto dirLight = std::make_shared<core::DirectionalLightNode>("Dir light");
    dirLight->setColor(glm::vec3(7.0f));
    dirLight->setTransform(utils::Transform::makeRotation(glm::quat(glm::vec3(-glm::quarter_pi<float>(), glm::quarter_pi<float>(), 0.f))));
    //scene0->sceneRootNode()->attach(dirLight);

    auto spotLight = std::make_shared<core::SpotLightNode>("Spot light");
    spotLight->setColor(glm::vec3(4.f));
    spotLight->setRadiuses(glm::vec2(5.f, 7.f));
    spotLight->setHalfAngles(glm::vec2(glm::pi<float>() / 6.f, glm::pi<float>() / 5.f));
    spotLight->setTransform(utils::Transform(1.f, glm::quat(glm::vec3(-glm::quarter_pi<float>(), glm::quarter_pi<float>(), 0.f)), glm::vec3(4.f, 3.f, 4.f)));
    //scene0->sceneRootNode()->attach(spotLight);

    auto iblLight = std::make_shared<core::ImageBasedLightNode>("IBL light");
    iblLight->setContribution(.1f);
    scene0->sceneRootNode()->attach(iblLight);

    auto planeDrawableNode = std::make_shared<core::DrawableNode>("Plane");
    planeDrawableNode->addDrawable(planeDrawable);
    scene0->sceneRootNode()->attach(planeDrawableNode);

    for (uint32_t z = 0u; z < teapotDrawables.size(); ++z)
        for (uint32_t x = 0u; x < teapotDrawables[z].size(); ++x)
        {
            auto teapotDrawableNode = std::make_shared<core::DrawableNode>("Teapot");
            teapotDrawableNode->addDrawable(teapotDrawables[z][x]);
            teapotDrawableNode->setTransform(utils::Transform::makeTranslation(glm::vec3(
                1.75f * (.5f + static_cast<float>(x) - static_cast<float>(teapotDrawables[z].size()) * .5f),
                0.f,
                1.75f * (.5f + static_cast<float>(z) - static_cast<float>(teapotDrawables.size()) * .5f))));
            scene0->sceneRootNode()->attach(teapotDrawableNode);
        }

    auto boxDrawableNode = std::make_shared<core::DrawableNode>("Box");
    boxDrawableNode->setTransform(utils::Transform::makeTranslation(glm::vec3(0.2f, 0.5f, 0.42f)) *
                                  utils::Transform::makeRotation(glm::quat(glm::vec3(-0.4f, 0.3f, 0.0f))));
    boxDrawableNode->addDrawable(boxDrawable);
    scene0->sceneRootNode()->attach(boxDrawableNode);

    auto tetraDrawableNode = std::make_shared<core::DrawableNode>("Tetra");
    tetraDrawableNode->setTransform(utils::Transform::makeTranslation(glm::vec3(-0.6f, 0.21f, -1.0f)));
    tetraDrawableNode->addDrawable(tetraDrawable);
    scene0->sceneRootNode()->attach(tetraDrawableNode);

    auto sphereDrawableNode = std::make_shared<core::DrawableNode>("Sphere");
    sphereDrawableNode->setTransform(utils::Transform::makeTranslation(glm::vec3(1.4f, 0.4f, -1.f)));
    sphereDrawableNode->addDrawable(sphereDrawable);
    scene0->sceneRootNode()->attach(sphereDrawableNode);

    auto bunnyDrawableNode = std::make_shared<core::DrawableNode>("Bunny");
    bunnyDrawableNode->setTransform(utils::Transform::makeTranslation(glm::vec3(1.3f, 0.01f, 1.0f)) *
                                      utils::Transform::makeRotation(glm::quat(glm::vec3(0.0f, -0.5f, 0.0f))));
    bunnyDrawableNode->addDrawable(bunnyDrawable);
    scene0->sceneRootNode()->attach(bunnyDrawableNode);

    auto suzanneDrawableNode = std::make_shared<core::DrawableNode>("Suzanne");
    suzanneDrawableNode->setTransform(utils::Transform::makeTranslation(glm::vec3(0.3f, 0.25f, -1.8f)) *
                                      utils::Transform::makeRotation(glm::quat(glm::vec3(-0.7f, 0.5f, 0.0f))));
    suzanneDrawableNode->addDrawable(suzanneDrawable);
    scene0->sceneRootNode()->attach(suzanneDrawableNode);

    auto soundNode = std::make_shared<core::SoundNode>("");
    auto source = soundNode->source();
    source->setBuffer(soundsManager->loadOrGetSound("D:/res/sounds/Footsteps.wav"));
    source->setLooping(true);
    scene0->sceneRootNode()->attach(soundNode);
    soundNode->setState(core::SoundState::Play);

    cameraNode0->attach(scene0->listenerNode());
}

void TestApplication::prepareSponzaScene()
{
    auto scene0 = loadGLTFScene("D:/res/Sponza/Sponza.gltf", m_renderWidget->defaultFrameBuffer());

    scene0->sceneRootNode()->attach(std::make_shared<core::CameraNode>("", m_renderWidget->defaultFrameBuffer()));

    auto light1 = std::make_shared<core::ImageBasedLightNode>("");
    light1->setContribution(0.1f);
    scene0->sceneRootNode()->attach(light1);

    auto light2 = std::make_shared<core::PointLightNode>("");
    light2->setTransform(utils::Transform::makeTranslation(glm::vec3(-.7f, 3.f, -.4f)));
    light2->setRadiuses(glm::vec2(5.f, 6.f));
    light2->setColor(glm::vec3(12.f));
    scene0->sceneRootNode()->attach(light2);

    auto light3 = std::make_shared<core::SpotLightNode>("");
    light3->setColor(glm::vec3(10.f));
    light3->setRadiuses(glm::vec2(5.f, 10.f));
    light3->setHalfAngles(glm::vec2(0.f, glm::pi<float>() / 6.f));
    light3->setTransform(utils::Transform(1.f, glm::quat(0.888179f, -0.260215f, 0.363439f, 0.106479f), glm::vec3(-.7f, 3.f, -.4f)));
    //scene0->sceneRootNode()->attach(light3);
}

void TestApplication::prepareChessScene()
{
    auto scene0 = loadGLTFScene("D:/res/ABeautifulGame/ABeautifulGame.gltf", m_renderWidget->defaultFrameBuffer());

    auto chessNode = std::make_shared<core::Node>("ChessNode");
    while (!scene0->sceneRootNode()->children().empty())
        chessNode->attach(scene0->sceneRootNode()->children().front());
    chessNode->setTransform(utils::Transform::makeScale(10.f));
    scene0->sceneRootNode()->attach(chessNode);

    //scene0->sceneRootNode()->attach(std::make_shared<core::CameraNode>("", m_renderWidget->defaultFrameBuffer()));

    auto light1 = std::make_shared<core::ImageBasedLightNode>("");
    scene0->sceneRootNode()->attach(light1);
}

void TestApplication::prepareEmptyRoomScene()
{
    auto graphicsEngineInstance = findEngine<core::GraphicsEngine>();
    auto audioEngineInstance = findEngine<core::AudioEngine>();

    auto texturesManager = graphicsEngineInstance->texturesManager();
    auto soundsManager = audioEngineInstance->soundsManager();

    auto graphicsRenderer = graphicsEngineInstance->graphicsRenderer();

    utils::MeshPainter boxPainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}},
                                                                {utils::VertexAttribute::Tangent, {4u, utils::VertexComponentType::Single}}
                                                                }));
    boxPainter.setVertexTransform(utils::Transform::makeScale(10.f));
    boxPainter.setTexCoordsTransform(utils::Transform::makeScale(4.f));
    boxPainter.drawCube();
    boxPainter.setVertexTransform(utils::Transform());
    boxPainter.setTexCoordsTransform(utils::Transform::makeScale(1.f));
    boxPainter.drawSphere();
    auto boxDrawable = std::make_shared<core::PBRDrawable>(graphicsRenderer->createVertexArray(boxPainter.mesh()), boxPainter.calculateBoundingBox());
    boxDrawable->setBaseColorMap(texturesManager->loadOrGetTexture("D:/res/textures/brick/bc.jpg"));
    boxDrawable->setNormalMap(texturesManager->loadOrGetTexture("D:/res/textures/brick/n.jpg"));
    boxDrawable->setMetalness(0.0f);
    boxDrawable->setRoughness(0.6f);

    auto scene0 = loadEmptyScene(SceneName);

    auto cameraNode0 = std::make_shared<core::CameraNode>("", m_renderWidget->defaultFrameBuffer());
    scene0->sceneRootNode()->attach(cameraNode0);

    auto dirLight = std::make_shared<core::DirectionalLightNode>("Dir light");
    dirLight->setColor(glm::vec3(7.0f));
    dirLight->setTransform(utils::Transform::makeRotation(glm::quat(glm::vec3(-glm::quarter_pi<float>(), glm::quarter_pi<float>(), 0.f))));
    scene0->sceneRootNode()->attach(dirLight);

    auto iblLight = std::make_shared<core::ImageBasedLightNode>("IBL light");
    iblLight->setContribution(.2f);
    scene0->sceneRootNode()->attach(iblLight);

    auto boxDrawableNode = std::make_shared<core::DrawableNode>("Box");
    boxDrawableNode->addDrawable(boxDrawable);
    scene0->sceneRootNode()->attach(boxDrawableNode);
}

void TestApplication::prepareRiggedFigureScene()
{
    auto scene0 = loadGLTFScene("D:/res/RiggedFigure/RiggedFigure.gltf", m_renderWidget->defaultFrameBuffer());

    scene0->sceneRootNode()->attach(std::make_shared<core::CameraNode>("", m_renderWidget->defaultFrameBuffer()));

    auto light1 = std::make_shared<core::ImageBasedLightNode>("");
    light1->setContribution(0.1f);
    scene0->sceneRootNode()->attach(light1);

    auto light2 = std::make_shared<core::PointLightNode>("");
    light2->setTransform(utils::Transform::makeTranslation(glm::vec3(-.7f, 3.f, -.4f)));
    light2->setRadiuses(glm::vec2(5.f, 6.f));
    light2->setColor(glm::vec3(12.f));
    //scene0->sceneRootNode()->attach(light2);

    auto light3 = std::make_shared<core::SpotLightNode>("");
    light3->setColor(glm::vec3(10.f));
    light3->setRadiuses(glm::vec2(5.f, 10.f));
    light3->setHalfAngles(glm::vec2(0.f, glm::pi<float>() / 6.f));
    light3->setTransform(utils::Transform(1.f, glm::quat(0.888179f, -0.260215f, 0.363439f, 0.106479f), glm::vec3(-.7f, 3.f, -.4f)));
    //scene0->sceneRootNode()->attach(light3);
}

void TestApplication::setShadowMode(simplex::core::ShadingMode value)
{
    auto graphicsEngine = findEngine<simplex::core::GraphicsEngine>();
    graphicsEngine->tempMode = value;
    graphicsEngine->upShading = true;
}

void TestApplication::setShadowFilter(simplex::core::ShadingFilter value)
{
    auto graphicsEngine = findEngine<simplex::core::GraphicsEngine>();
    graphicsEngine->tempFilter = value;
    graphicsEngine->upShading = true;
}

#include <utils/logger.h>
#include <utils/transform.h>
#include <utils/meshpainter.h>
#include <utils/mesh.h>
#include <utils/glm/gtc/color_space.hpp>

#include <core/applicationbase.h>
#include <core/background.h>
#include <core/graphicsengine.h>
#include <core/graphicsrendererbase.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/cameranode.h>
#include <core/drawablenode.h>
#include <core/drawable.h>
#include <core/imagebasedllightnode.h>
#include <core/directionallightnode.h>
#include <core/pointlightnode.h>
#include <core/spotlightnode.h>
#include <core/mesh.h>
#include <core/material.h>
#include <core/shadow.h>
#include <core/nodecollector.h>

#include <graphics_glfw/glfwwidget.h>

struct Cone
{
    glm::vec3 origin;
    glm::vec3 direction;
    float height;
    float angle;

    Cone(const glm::vec3& o, const glm::vec3& d, float h, float a)
        : origin(o)
        , direction(glm::normalize(d))
        , height(h)
        , angle(a)
    {}

    bool isPointInside(const glm::vec3& v) const
    {
        const auto CtoV = v - origin;
        const float t = glm::dot(direction, CtoV);

        return (t <= height) && (t >= cos(angle) * length(CtoV));
    }
};

glm::vec3 coneOrigin(const Cone& c)
{
    return c.origin;
}

glm::vec3 coneDirection(const Cone& c)
{
    return c.direction;
}

Cone transformCone(const simplex::utils::Transform& t, const Cone& c)
{
    return Cone{
        t.transformPoint(c.origin),
        t.transformVector(c.direction),
        t.scale * c.height,
        c.angle };
}

#define INSIDE 1
#define OUTSIDE -1
#define INTERSECT 0

using Range = glm::vec2;
using uint = uint32_t;
using vec3 = glm::vec3;

Range makeRange(float t0, float t1)
{
    return (t0 < t1) ? Range(t0, t1) : Range(t1, t0);
}

Range makeRange(float t)
{
    return Range(t, t);
}

Range makeEmptyRange()
{
    return Range(FLT_MAX, -FLT_MAX);
}

float rangeStart(const Range& r)
{
    return r[0u];
}

float rangeEnd(const Range& r)
{
    return r[1u];
}

Range rangeExpand(const Range& r, float t)
{
    return makeRange(glm::min(r[0u], t), glm::max(r[1u], t));
}

bool rangeLessThan(const Range& r, float t)
{
    return glm::all(glm::lessThan(r, glm::vec2(t)));
}

bool rangeGreaterThan(const Range& r, float t)
{
    return glm::all(glm::greaterThan(r, glm::vec2(t)));
}

bool rangeIsPointInside(const Range& r, float t)
{
    return (r[0u] <= t) && (t <= r[1u]);
}

int rangeClassifyRange(const Range& r0, Range& r1)
{
    if (rangeIsPointInside(r0, rangeStart(r1)) && rangeIsPointInside(r0, rangeEnd(r1)))
        return INSIDE;

    if (rangeGreaterThan(r1, rangeEnd(r0)) || rangeLessThan(r1, rangeStart(r0)))
        return OUTSIDE;

    return INTERSECT;
}

Range boundingBoxProjectOnLine(const simplex::utils::BoundingBox& bb, const simplex::utils::Line& l)
{
    auto r = bb.projectOnLine(l);
    return makeRange(r.nearValue(), r.farValue());
}

glm::vec3 boundingBoxCenter(const simplex::utils::BoundingBox& bb)
{
    return bb.center();
}

glm::vec3 boundingBoxHalfSize(const simplex::utils::BoundingBox& bb)
{
    return bb.halfSize();
}

glm::vec3 boundingBoxMinPoint(const simplex::utils::BoundingBox& bb)
{
    return bb.minPoint();
}

glm::vec3 boundingBoxMaxPoint(const simplex::utils::BoundingBox& bb)
{
    return bb.maxPoint();
}

simplex::utils::Line makeLine(vec3 o, vec3 d)
{
    return simplex::utils::Line(o, d);
}

Range coneProjectOnLine(const Cone& c, const simplex::utils::Line& l)
{
    const glm::vec3 coneOrig = c.origin;
    const glm::vec3 coneDir = c.direction;
    const float coneH = c.height;
    const glm::vec3 coneEnd = coneOrig + coneDir * coneH;

    const glm::vec3 crossVec = glm::cross(coneDir, l.direction());
    const float crossVecLen = glm::length(crossVec);

    Range result = makeRange(l.projectOn(coneOrig));

    if (crossVecLen < simplex::utils::epsilon<float>())
    {
        result = rangeExpand(result, l.projectOn(coneEnd));
    }
    else
    {
        const glm::vec3 tangent = glm::cross(coneDir, crossVec / crossVecLen) * sin(c.angle) * coneH;
        result = rangeExpand(result, l.projectOn(coneEnd + tangent));
        result = rangeExpand(result, l.projectOn(coneEnd - tangent));
    }

    return result;
}

int coneClassifyBoundingBox(const Cone& c, const simplex::utils::BoundingBox& bb)
{
    bool inside = true;

    const glm::vec3 coneDir = coneDirection(c);

    simplex::utils::Line l(coneOrigin(c), coneDir);
    int classify = rangeClassifyRange(
        //makeRange(0.0f, c.height),
        coneProjectOnLine(c, l),
        boundingBoxProjectOnLine(bb, l));
    if (classify == OUTSIDE)
        return OUTSIDE;
    else if (classify == INTERSECT)
        inside = false;

    const vec3 boxCenter = boundingBoxCenter(bb);
    const vec3 boxHalfSize = boundingBoxHalfSize(bb);
    for (uint i = 0u; i < 3u; ++i)
    {
        vec3 dir(0.0f);
        dir[i] = 1.0f;

        l = makeLine(boxCenter, dir);
        classify = rangeClassifyRange(
            coneProjectOnLine(c, l),
            //makeRange(boxCenter[i] - boxHalfSize[i], boxCenter[i] + boxHalfSize[i]));
            boundingBoxProjectOnLine(bb, l));
        if (classify == OUTSIDE)
            return OUTSIDE;
        else if (classify == INTERSECT)
            inside = false;

        dir = glm::cross(dir, coneDir);
        if (glm::length(dir) < simplex::utils::epsilon<float>())
            continue;

        l = makeLine(boxCenter, dir);
        classify = rangeClassifyRange(
            coneProjectOnLine(c, l),
            boundingBoxProjectOnLine(bb, l));
        if (classify == OUTSIDE)
            return OUTSIDE;
        else if (classify == INTERSECT)
            inside = false;
    }

    return inside ? INSIDE : INTERSECT;
}

struct LineIntersectConeResult { uint32_t count; float t[2u]; };
LineIntersectConeResult lineIntersectCone(const simplex::utils::Line& l, const Cone& c)
{
    const glm::quat rot = glm::quat(c.direction, glm::vec3(0.0f, 0.0f, 1.0f));
    const glm::vec3 trans = -(rot * c.origin);

    const simplex::utils::Line newL = simplex::utils::Transform(1.0f, rot, trans) * l;
    const glm::vec3 lo = newL.origin();
    const glm::vec3 ld = newL.direction();

    const Range r = glm::vec2(0.0f, c.height);

    const float angle = c.angle;
    const float cosAngle = glm::cos(angle);
    const float tanAngle = glm::tan(angle);
    const float tanAngleSqr = tanAngle * tanAngle;

    const float coefA = ld.x * ld.x + ld.y * ld.y - tanAngleSqr * ld.z * ld.z;
    const float coefB = 2.0f * (lo.x * ld.x + lo.y * ld.y - tanAngleSqr * lo.z * ld.z);
    const float coefC = lo.x * lo.x + lo.y * lo.y - tanAngleSqr * lo.z * lo.z;

    uint32_t resCount = 0u;
    float resT[2u] = {0.0f, 0.0f};

    if (abs(coefA) < simplex::utils::epsilon<float>())
    {
        if (abs(coefB) > simplex::utils::epsilon<float>())
        {
            float t = -coefC / coefB;
            if (rangeIsPointInside(r, lo.z + t * ld.z))
                resT[resCount++] = t;
        }
    }
    else
    {
        const float d = coefB * coefB - 4.0f * coefA * coefC;
        const float inv2A = 1.0f / (2.0f * coefA);

        if (d < 0.0f)
            ;
        else if (d < simplex::utils::epsilon<float>())
        {
            if (abs(ld.z) > cosAngle)
                resT[resCount++] = -coefB * inv2A;
        }
        else
        {
            const float sqrtD = sqrt(d);

            const float t0 = (-coefB + sqrtD) * inv2A;
            if (rangeIsPointInside(r, lo.z + t0 * ld.z))
                resT[resCount++] = t0;

            const float t1 = (-coefB - sqrtD) * inv2A;
            if (rangeIsPointInside(r, lo.z + t1 * ld.z))
                resT[resCount++] = t1;
        }
    }

    if (resCount == 1u)
        resT[resCount++] = (r[1u] - lo.z) / ld.z;

    if ((resCount > 0u) && (resT[0u] > resT[1u]))
        std::swap(resT[0u], resT[1u]);

    return { resCount, {resT[0u], resT[1u]}};
}



Cone cone(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f), 6.f, glm::quarter_pi<float>() / 2.0f);
std::weak_ptr<simplex::core::Material> boxMat;
std::weak_ptr<simplex::core::Material> coneMat;
std::weak_ptr<simplex::core::DrawableNode> boxDN;
std::weak_ptr<simplex::core::ImageBasedLightNode> IBLNodeWeak;

static std::shared_ptr<simplex::core::Scene> createScene(
    const std::string& name,
    const std::shared_ptr<simplex::core::graphics::RendererBase>& renderer)
{
    renderer->makeCurrent();

    auto scene = simplex::core::Scene::createEmpty(name);

    simplex::utils::MeshPainter planePainter(simplex::utils::Mesh::createEmptyMesh({
        {simplex::utils::VertexAttribute::Position, {3u, simplex::utils::VertexComponentType::Single}},
        {simplex::utils::VertexAttribute::Normal, {3u, simplex::utils::VertexComponentType::Single}},
        {simplex::utils::VertexAttribute::TexCoords, {2u, simplex::utils::VertexComponentType::Single}},
        {simplex::utils::VertexAttribute::Tangent, {4u, simplex::utils::VertexComponentType::Single}}
        }));
    planePainter.setVertexTransform(simplex::utils::Transform::makeScale(10.f));
    planePainter.setTexCoordsTransform(simplex::utils::Transform::makeScale(5.f));
    planePainter.drawPlane();
    auto planeBoundingBox = planePainter.calculateBoundingBox();

    auto planeMesh = std::make_shared<simplex::core::Mesh>(planePainter.mesh(), planeBoundingBox);

    auto planeMaterial = std::make_shared<simplex::core::Material>();
    planeMaterial->setMaterialMap(
        simplex::core::MaterialMapTarget::BaseColor,
        std::make_shared<simplex::core::MaterialMap>("./resources/textures/brick/bc.jpg"));
    planeMaterial->setMaterialMap(
        simplex::core::MaterialMapTarget::Normal,
        std::make_shared<simplex::core::MaterialMap>("./resources/textures/brick/n.jpg"));
    planeMaterial->setMetalness(0.f);
    planeMaterial->setRoughness(.3f);
    planeMaterial->setNormalMapScale(.3f);
    planeMaterial->setDoubleSided(false);

    auto planeDrawableNode = std::make_shared<simplex::core::DrawableNode>("");
    planeDrawableNode->addDrawable(
        std::make_shared<simplex::core::Drawable>(planeMesh, planeMaterial));
    planeDrawableNode->setTransform(
        simplex::utils::Transform::makeRotation(glm::quat(glm::vec3(-glm::half_pi<float>(), 0.f, 0.f))) *
        simplex::utils::Transform::makeScale(1.f));
    scene->sceneRootNode()->attach(planeDrawableNode);

    static const size_t NumGeometries = 4u;
    static const std::array<glm::vec3, NumGeometries> nodePositions {
        glm::vec3(-1.f, 0.f, -1.f),
        glm::vec3(-1.f, 0.f, +1.f),
        glm::vec3(+1.f, 1.f, +1.f),
        glm::vec3(+1.f, 0.f, -1.f) };
    static const std::array<simplex::utils::MeshPainter& (simplex::utils::MeshPainter::*)(), NumGeometries> painterFunctions{
        &simplex::utils::MeshPainter::drawBunny,
        &simplex::utils::MeshPainter::drawTeapot,
        &simplex::utils::MeshPainter::drawSuzanne,
        &simplex::utils::MeshPainter::drawTetrahedron
    };
    static const std::array<glm::vec4, NumGeometries> materialBaseColors{
        glm::convertSRGBToLinear(glm::vec4(.5f, .5f, 1.f, 1.f)),
        glm::convertSRGBToLinear(glm::vec4(1.f, 0.f, 0.f, .2f)),
        glm::convertSRGBToLinear(glm::vec4(0.f, 1.f, 0.f, 1.f)),
        glm::convertSRGBToLinear(glm::vec4(0.f, 0.f, 1.f, .3f)) };

    for (size_t i = 0u; i < NumGeometries; ++i)
    {
        simplex::utils::MeshPainter painter(simplex::utils::Mesh::createEmptyMesh({
        {simplex::utils::VertexAttribute::Position, {3u, simplex::utils::VertexComponentType::Single}},
        {simplex::utils::VertexAttribute::Normal, {3u, simplex::utils::VertexComponentType::Single}} }));

        (painter.*(painterFunctions[i]))();
        
        auto mesh = std::make_shared<simplex::core::Mesh>(painter.mesh(), painter.calculateBoundingBox());

        auto material = std::make_shared<simplex::core::Material>();
        material->setBaseColor(materialBaseColors[i]);
        material->setMetalness(0.f);
        material->setRoughness(.2f);

        auto drawable = std::make_shared<simplex::core::Drawable>(mesh, material);

        auto drawableNode = std::make_shared<simplex::core::DrawableNode>("");
        drawableNode->addDrawable(drawable);
        drawableNode->setTransform(simplex::utils::Transform::makeTranslation(nodePositions[i]));
        scene->sceneRootNode()->attach(drawableNode);
    }
    
    //cone
    simplex::utils::MeshPainter conePainter(simplex::utils::Mesh::createEmptyMesh({
        {simplex::utils::VertexAttribute::Position, {3u, simplex::utils::VertexComponentType::Single}},
        {simplex::utils::VertexAttribute::Normal, {3u, simplex::utils::VertexComponentType::Single}} }));
    conePainter.setVertexTransform(simplex::utils::Transform(1.0f, glm::quat(glm::vec3(0.0f, 0.0f, -1.0f), cone.direction), cone.origin));
    conePainter.drawCone(cone.height, cone.angle, 32u);

    auto coneMesh = std::make_shared<simplex::core::Mesh>(conePainter.mesh(), conePainter.calculateBoundingBox());

    auto coneMaterial = std::make_shared<simplex::core::Material>();
    coneMaterial->setBaseColor(glm::vec4(0.f, 0.f, 1.f, .5f));
    coneMaterial->setMetalness(0.f);
    coneMaterial->setRoughness(.2f);
    coneMat = coneMaterial;

    auto sphDrawable = std::make_shared<simplex::core::Drawable>(coneMesh, coneMaterial);

    auto coneDrawableNode = std::make_shared<simplex::core::DrawableNode>("");
    coneDrawableNode->addDrawable(sphDrawable);
    //scene->sceneRootNode()->attach(coneDrawableNode);
    //

    auto cameraNode = std::make_shared<simplex::core::CameraNode>("");
    scene->sceneRootNode()->attach(cameraNode);

    auto pointLightNode = std::make_shared<simplex::core::PointLightNode>("");
    pointLightNode->setTransform(simplex::utils::Transform::makeTranslation(glm::vec3(0.f, 2.f, 0.f)));
    pointLightNode->setColor(glm::vec3(3.f));
    pointLightNode->setRadiuses(glm::vec2(2.5f, 3.f));
    scene->sceneRootNode()->attach(pointLightNode);

    auto spotLightNode = std::make_shared<simplex::core::SpotLightNode>("");
    spotLightNode->setTransform(simplex::utils::Transform::makeLookAt(
        glm::vec3(1.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f)).inverted());
    spotLightNode->setColor(glm::vec3(3.f));
    spotLightNode->setRadiuses(glm::vec2(2.5f, 3.f));
    //scene->sceneRootNode()->attach(spotLightNode);

    auto IBLNode = std::make_shared<simplex::core::ImageBasedLightNode>("");
    IBLNode->setContribution(1.f);
    scene->sceneRootNode()->attach(IBLNode);
    IBLNodeWeak = IBLNode;

    auto directionalLightNode = std::make_shared<simplex::core::DirectionalLightNode>("");
    directionalLightNode->setTransform(simplex::utils::Transform::makeRotation(
        glm::quat(glm::normalize(glm::vec3(1.f)), glm::normalize(glm::vec3(0.f, 0.f, -1)))));
    directionalLightNode->shadow().setMode(simplex::core::ShadingMode::Opaque);
    directionalLightNode->shadow().setFilter(simplex::core::ShadingFilter::Point);
    //scene->sceneRootNode()->attach(directionalLightNode);

    return scene;
}

void createSpheres()
{
    ////sphere
    //simplex::utils::MeshPainter sphPainter(simplex::utils::Mesh::createEmptyMesh({
    //    {simplex::utils::VertexAttribute::Position, {3u, simplex::utils::VertexComponentType::Single}} }));
    //sphPainter.drawCube();

    //auto sphMesh = std::make_shared<simplex::core::Mesh>(sphPainter.mesh(), sphPainter.calculateBoundingBox());

    //auto sphMaterial = std::make_shared<simplex::core::Material>();
    //sphMaterial->setMetalness(0.f);
    //sphMaterial->setRoughness(.2f);
    //boxMat = sphMaterial;

    //auto sphDrawable = std::make_shared<simplex::core::Drawable>(sphMesh, sphMaterial);

    //auto sphDrawableNode1 = std::make_shared<simplex::core::DrawableNode>("");
    //sphDrawableNode1->setTransform(simplex::utils::Transform::makeScale(0.0f));
    //sphDrawableNode1->addDrawable(sphDrawable);
    //boxDN = sphDrawableNode1;

    //if (auto window = simplex::graphics_glfw::GLFWWidget::getOrCreate("Simple scene"))
    //    if (auto graphicsEngine = window->graphicsEngine())
    //    {
    //        auto& scene = graphicsEngine->scene();
    //        scene->sceneRootNode()->attach(sphDrawableNode1);
    //    }
}

static bool isWPressed = false;
static bool isSPressed = false;
static bool isAPressed = false;
static bool isDPressed = false;
static bool isQPressed = false;
static bool isEPressed = false;
static bool isLeftPressed = false;
static bool isRightPressed = false;
static bool isUpPressed = false;
static bool isDownPressed = false;
static bool isSpacePressed = false;
static bool isLShiftPressed = false;
static glm::vec2 cameraAngles(-0.37f, 5.5f);
static glm::vec3 cameraPosition(-2.94f, 1.83f, 2.44f);

static void keyCallback(
    simplex::core::graphics::KeyState keyState,
    simplex::core::graphics::KeyCode keyCode,
    const simplex::core::graphics::KeyModifiers&)
{
    switch (keyCode)
    {
    case simplex::core::graphics::KeyCode::W:
    {
        isWPressed = keyState != simplex::core::graphics::KeyState::Released;
        break;
    }
    case simplex::core::graphics::KeyCode::S:
    {
        isSPressed = keyState != simplex::core::graphics::KeyState::Released;
        break;
    }
    case simplex::core::graphics::KeyCode::A:
    {
        isAPressed = keyState != simplex::core::graphics::KeyState::Released;
        break;
    }
    case simplex::core::graphics::KeyCode::D:
    {
        isDPressed = keyState != simplex::core::graphics::KeyState::Released;
        break;
    }
    case simplex::core::graphics::KeyCode::Q:
    {
        isQPressed = keyState != simplex::core::graphics::KeyState::Released;
        break;
    }
    case simplex::core::graphics::KeyCode::E:
    {
        isEPressed = keyState != simplex::core::graphics::KeyState::Released;
        break;
    }
    case simplex::core::graphics::KeyCode::Left:
    {
        isLeftPressed = keyState != simplex::core::graphics::KeyState::Released;
        break;
    }
    case simplex::core::graphics::KeyCode::Right:
    {
        isRightPressed = keyState != simplex::core::graphics::KeyState::Released;
        break;
    }
    case simplex::core::graphics::KeyCode::Up:
    {
        isUpPressed = keyState != simplex::core::graphics::KeyState::Released;
        break;
    }
    case simplex::core::graphics::KeyCode::Down:
    {
        isDownPressed = keyState != simplex::core::graphics::KeyState::Released;
        break;
    }
    case simplex::core::graphics::KeyCode::LeftShift:
    {
        isLShiftPressed = keyState != simplex::core::graphics::KeyState::Released;
        break;
    }
    case simplex::core::graphics::KeyCode::Space:
    {
        if (keyState == simplex::core::graphics::KeyState::Pressed)
        {
            if (auto window = simplex::graphics_glfw::GLFWWidget::getOrCreate("Simple scene"))
                if (auto graphicsEngine = window->graphicsEngine())
                {
                    graphicsEngine->setF();
                    isSpacePressed = true;
                }
        }

        break;
    }
    default:
        break;
    }
}

static void updateCallback(uint64_t time, uint32_t dt)
{
    glm::vec2 deltaAngles(0.f, 0.f);

    if (isUpPressed)
        deltaAngles.x += 1.f;

    if (isDownPressed)
        deltaAngles.x -= 1.f;

    if (isLeftPressed)
        deltaAngles.y += 1.f;

    if (isRightPressed)
        deltaAngles.y -= 1.f;

    if (glm::length(deltaAngles) > .1f)
        cameraAngles += glm::normalize(deltaAngles) * static_cast<float>(dt) * .001f;

    cameraAngles.x = glm::max(cameraAngles.x, -glm::half_pi<float>());
    cameraAngles.x = glm::min(cameraAngles.x, +glm::half_pi<float>());
    cameraAngles.y = glm::mod(cameraAngles.y, glm::two_pi<float>());

    auto cameraRotation = simplex::utils::Transform::makeRotation(glm::quat(glm::vec3(cameraAngles, 0.0f)));
    auto cameraFowrardDir = cameraRotation.transformVector(glm::vec3(0.0f, 0.0f, -1.0f));
    auto cameraRightDir = cameraRotation.transformVector(glm::vec3(1.0f, 0.0f, 0.0f));
    auto cameraUpDir = cameraRotation.transformVector(glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 cameraDir(0.f);

    if (isWPressed)
        cameraDir += cameraFowrardDir;

    if (isSPressed)
        cameraDir -= cameraFowrardDir;

    if (isDPressed)
        cameraDir += cameraRightDir;

    if (isAPressed)
        cameraDir -= cameraRightDir;

    if (isQPressed)
        cameraDir -= cameraUpDir;

    if (isEPressed)
        cameraDir += cameraUpDir;

    const float vel = isLShiftPressed ? .03f : .003f;
    if (glm::length(cameraDir) > .1f)
        cameraPosition += glm::normalize(cameraDir) * static_cast<float>(dt) * vel;

    auto cameraTranslation = simplex::utils::Transform::makeTranslation(cameraPosition);
    auto cameraTransform = cameraTranslation * cameraRotation;

    auto envRotation = glm::quat(glm::vec3(0.f, time * 0.001f * 0.02f, 0.0f));

    if (auto window = simplex::graphics_glfw::GLFWWidget::getOrCreate("Simple scene"))
        if (auto graphicsEngine = window->graphicsEngine())
        {
            auto scene = graphicsEngine->scene();
            simplex::core::NodeCollector<simplex::core::CameraNode> cameraCollector;
            scene->sceneRootNode()->acceptDown(cameraCollector);
            cameraCollector.nodes().front()->setTransform(cameraTransform);

            scene->background()->setRotation(envRotation);
            if (auto IBLNode = IBLNodeWeak.lock())
                IBLNode->setTransform(simplex::utils::Transform::makeRotation(envRotation).inverted());
        }

    //if (isSpacePressed)
    //{
    //    isSpacePressed = false;

    //    boxDN.lock()->setTransform(cameraTransform);

    //    auto c = coneClassifyBoundingBox(
    //        transformCone(cameraTransform.inverted(), cone),
    //        simplex::utils::BoundingBox::fromCenterHalfSize(glm::vec3(0.f), glm::vec3(.5f)));

    //    if (c == INSIDE)
    //        boxMat.lock()->setBaseColor(glm::vec4(0.f, 1.f, 0.f, .5f));
    //    else if (c == OUTSIDE)
    //        boxMat.lock()->setBaseColor(glm::vec4(0.f, 0.f, 1.f, .5f));
    //    else
    //        boxMat.lock()->setBaseColor(glm::vec4(1.f, 0.f, 0.f, .5f));
    //}
}

int main(int argc, char* argv[])
{
    if (!simplex::core::ApplicationBase::initialize(
        []() { return simplex::graphics_glfw::GLFWWidget::time(); },
        []() { simplex::graphics_glfw::GLFWWidget::pollEvents(); }
    ))
    {
        LOG_CRITICAL << "Failed to initialize application";
        return 0;
    }

    auto window = simplex::graphics_glfw::GLFWWidget::getOrCreate("Simple scene");
    window->engine()->setScene(createScene("SimpleScene", window->graphicsEngine()->graphicsRenderer()));
    window->setKeyCallback(keyCallback);
    window->setUpdateCallback(updateCallback);

    createSpheres();

    auto& app = simplex::core::ApplicationBase::instance();
    app.registerDevice(window);
    app.run();
    app.unregisterDevice(window);

    return 0;
}
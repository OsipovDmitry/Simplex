#include <utils/logger.h>
#include <utils/mesh.h>
#include <utils/meshpainter.h>

#include <core/igraphicsrenderer.h>
#include <core/graphicsengine.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/drawable.h>
#include <core/uniform.h>

#include "sceneprivate.h"

namespace simplex
{
namespace core
{

const std::string SceneRootNodeName = "SceneRootNode";

Scene::~Scene() = default;

const std::string &Scene::name() const
{
    return m_->name();
}

std::weak_ptr<const GraphicsEngine> Scene::graphicsEngine() const
{
    return const_cast<Scene*>(this)->graphicsEngine();
}

std::weak_ptr<GraphicsEngine> Scene::graphicsEngine()
{
    return m_->graphicsEngine();
}

std::shared_ptr<const SceneRootNode> Scene::sceneRootNode() const
{
    return const_cast<Scene*>(this)->sceneRootNode();
}

std::shared_ptr<SceneRootNode> Scene::sceneRootNode()
{
    return m_->sceneRootNode();
}

graphics::PConstTexture Scene::backgroundTexture() const
{
    return uniform_cast<graphics::PConstTexture>(m_->backgroundScreenQuadDrawable()->uniform(graphics::UniformId::BackgroundColorMap))->data();
}

void Scene::setBackgroundTexture(const graphics::PConstTexture &value)
{
    uniform_cast<graphics::PConstTexture>(m_->backgroundScreenQuadDrawable()->uniform(graphics::UniformId::BackgroundColorMap))->data() = value;
}

float Scene::backgroundRoughness() const
{
    return uniform_cast<float>(m_->backgroundScreenQuadDrawable()->uniform(graphics::UniformId::Roughness))->data();
}

void Scene::setBackgroundRoughness(float value)
{
    uniform_cast<float>(m_->backgroundScreenQuadDrawable()->uniform(graphics::UniformId::Roughness))->data() = value;
}

Scene::Scene(const std::weak_ptr<GraphicsEngine> &graphicsEngine, const std::string &name)
    : m_(std::make_unique<ScenePrivate>(graphicsEngine, name, std::shared_ptr<SceneRootNode>(new SceneRootNode(SceneRootNodeName))))
{
    static const std::unordered_map<utils::VertexAttribute, std::tuple<uint32_t, utils::VertexComponentType>> s_screenDrawableVertexDeclaration {
        {utils::VertexAttribute::Position, {2u, utils::VertexComponentType::Single}} };

    if (graphicsEngine.expired())
        LOG_CRITICAL << "Graphics engine is expired";

    if (ScenePrivate::defaultBacgroundTexture().expired())
        LOG_CRITICAL << "Background default texture is expired";

    auto screeQuadVertexArray = graphicsEngine.lock()->graphicsRenderer()->createVertexArray(
                utils::MeshPainter(utils::Mesh::createEmptyMesh(s_screenDrawableVertexDeclaration)).drawScreenQuad().mesh());

    m_->backgroundScreenQuadDrawable() = std::make_shared<Drawable>(screeQuadVertexArray);
    m_->backgroundScreenQuadDrawable()->getOrCreateUniform(graphics::UniformId::BackgroundColorMap) = makeUniform(
                ScenePrivate::defaultBacgroundTexture().lock());
    m_->backgroundScreenQuadDrawable()->getOrCreateUniform(graphics::UniformId::Roughness) = makeUniform(.8f);

}



}
}

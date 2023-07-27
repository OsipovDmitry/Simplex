#ifndef GRAPHICSENGINE_H
#define GRAPHICSENGINE_H

#include <memory>
#include <vector>

#include <utils/noncopyble.h>
#include <utils/glm/vec4.hpp>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>
#include <core/iengine.h>

namespace simplex
{
namespace core
{

class GraphicsEnginePrivate;
class CORE_SHARED_EXPORT GraphicsEngine : public IEngine
{
    NONCOPYBLE(GraphicsEngine)

public:
    GraphicsEngine(const std::string&, std::shared_ptr<graphics::IRenderer>);
    ~GraphicsEngine() override;

    const std::string &name() const override;
    void update(uint64_t time, uint32_t dt) override;

    std::shared_ptr<graphics::IRenderer> graphicsRenderer() const;
    std::shared_ptr<RenderProgramsManager> renderProgramsManager() const;
    std::shared_ptr<RenderSurface> defaultRenderSurface() const;

    const std::vector<std::shared_ptr<Scene>> &scenes() const;
    std::shared_ptr<Scene> addNewScene(const std::string&);
    void removeScene(std::shared_ptr<Scene>);

    std::shared_ptr<StandardDrawable> createStandardDrawable(std::shared_ptr<graphics::IVertexArray>,
                                                             const glm::vec4& baseColor,
                                                             float metalness,
                                                             float roughness,
                                                             std::shared_ptr<const graphics::ITexture> baseColorMap,
                                                             std::shared_ptr<const graphics::ITexture> metalnessMap,
                                                             std::shared_ptr<const graphics::ITexture> roughnessMap,
                                                             std::shared_ptr<const graphics::ITexture> normalMap);

    void setF(int);

private:
    std::unique_ptr<GraphicsEnginePrivate> m_;

};

}
}

#endif // GRAPHICSENGINE_H

#ifndef CORE_GRAPHICSENGINE_H
#define CORE_GRAPHICSENGINE_H

#include <utils/noncopyble.h>
#include <utils/pimpl.h>

#include <core/coreglobal.h>
#include <core/iengine.h>

namespace simplex
{
namespace core
{

class GraphicsEnginePrivate;
class CORE_SHARED_EXPORT GraphicsEngine : public IEngine
{
    NONCOPYBLE(GraphicsEngine)
    PRIVATE_IMPL(GraphicsEngine)

public:
    GraphicsEngine(const std::string&, const std::shared_ptr<graphics::RendererBase>&);
    ~GraphicsEngine() override;

    const std::string& name() const override;

    std::shared_ptr<IRenderer> renderer() override final;
    std::shared_ptr<const IRenderer> renderer() const override final;

    std::shared_ptr<graphics::RendererBase> graphicsRenderer();
    std::shared_ptr<const graphics::RendererBase> graphicsRenderer() const;

    std::shared_ptr<TexturesLoader> texturesLoader();
    std::shared_ptr<const TexturesLoader> texturesLoader() const;

    std::shared_ptr<ProgramsLoader> programsLoader();
    std::shared_ptr<const ProgramsLoader> programsLoader() const;

    std::shared_ptr<ScenesLoader> scenesLoader();
    std::shared_ptr<const ScenesLoader> scenesLoader() const;

    void update(const std::shared_ptr<Scene>& scene, uint64_t time, uint32_t dt);

private:
    std::unique_ptr<GraphicsEnginePrivate> m_;

};

}
}

#endif // CORE_GRAPHICSENGINE_H

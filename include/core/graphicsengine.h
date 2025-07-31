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

    std::shared_ptr<Scene> scene() override final;
    std::shared_ptr<const Scene> scene() const override final;
    void setScene(const std::shared_ptr<Scene>&) override final;

    std::shared_ptr<graphics::RendererBase> graphicsRenderer();
    std::shared_ptr<const graphics::RendererBase> graphicsRenderer() const;

    std::shared_ptr<TexturesManager> texturesManager();
    std::shared_ptr<const TexturesManager> texturesManager() const;

    std::shared_ptr<ProgramsManager> programsManager();
    std::shared_ptr<const ProgramsManager> programsManager() const;

    void update(uint64_t time, uint32_t dt, debug::SceneInformation&);

    void setF(int);
    bool b = false;
    float ssaoContrib = 0.f;

    ShadingMode tempMode;
    ShadingFilter tempFilter;
    bool upShading = false;

private:
    std::unique_ptr<GraphicsEnginePrivate> m_;

};

}
}

#endif // CORE_GRAPHICSENGINE_H

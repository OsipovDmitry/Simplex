#ifndef CORE_GRAPHICSENGINE_H
#define CORE_GRAPHICSENGINE_H

#include <memory>

#include <utils/noncopyble.h>
#include <utils/pimpl.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>
#include <core/iengine.h>

namespace simplex
{
namespace core
{

class GraphicsEnginePrivate;
class CORE_SHARED_EXPORT GraphicsEngine : public std::enable_shared_from_this<GraphicsEngine>, public IEngine
{
    NONCOPYBLE(GraphicsEngine)
    PRIVATE_IMPL(GraphicsEngine)

public:
    GraphicsEngine(const std::string&, std::shared_ptr<graphics::IRenderer>);
    ~GraphicsEngine() override;

    const std::string &name() const override;
    void update(const std::shared_ptr<IRenderWidget>&,
                const std::shared_ptr<Scene>&,
                uint64_t time,
                uint32_t dt,
                debug::SceneInformation&) override;

    std::shared_ptr<graphics::IRenderer> graphicsRenderer();
    std::shared_ptr<const graphics::IRenderer> graphicsRenderer() const;

    std::shared_ptr<TexturesManager> texturesManager();
    std::shared_ptr<const TexturesManager> texturesManager() const;

    std::shared_ptr<ProgramsManager> programsManager();
    std::shared_ptr<const ProgramsManager> programsManager() const;

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

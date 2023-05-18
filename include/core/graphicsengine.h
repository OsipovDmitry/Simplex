#ifndef GRAPHICSENGINE_H
#define GRAPHICSENGINE_H

#include <memory>
#include <vector>

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
class CORE_SHARED_EXPORT GraphicsEngine : public IEngine
{
    NONCOPYBLE(GraphicsEngine)
    PIMPL(GraphicsEngine)

public:
    GraphicsEngine(const std::string&, std::shared_ptr<IGraphicsRenderer>);
    ~GraphicsEngine() override;

    const std::string &name() const override;
    void update(uint64_t time, uint32_t dt) override;

    std::shared_ptr<IGraphicsRenderer> graphicsRenderer() const;

    const std::vector<std::shared_ptr<Scene>> &scenes() const;
    void addScene(std::shared_ptr<Scene>);
    void removeScene(std::shared_ptr<Scene>);

private:
    std::unique_ptr<GraphicsEnginePrivate> m_;

    static float s_minZNear;
    static float s_maxZFat;
};

}
}

#endif // GRAPHICSENGINE_H

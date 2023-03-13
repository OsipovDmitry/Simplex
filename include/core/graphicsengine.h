#ifndef GRAPHICSENGINE_H
#define GRAPHICSENGINE_H

#include <memory>

#include <utils/noncopyble.h>

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
    GraphicsEngine(const std::string&, std::shared_ptr<IGraphicsRenderer>);
    ~GraphicsEngine() override;

    const std::string &name() const override;
    void update() override;

private:
    std::unique_ptr<GraphicsEnginePrivate> m_;
};

}
}

#endif // GRAPHICSENGINE_H

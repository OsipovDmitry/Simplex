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

class CORE_SHARED_EXPORT GraphicsEngine : public IEngine
{
    NONCOPYBLE(GraphicsEngine)
public:
    GraphicsEngine(std::shared_ptr<IGraphicsRenderer>);

    void update() override;
};

}
}

#endif // GRAPHICSENGINE_H

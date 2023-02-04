#ifndef GRAPHICSENGINE_H
#define GRAPHICSENGINE_H

#include <memory>

#include <core/forwarddecl.h>

class GraphicsEngine : public IEngine
{
public:
    GraphicsEngine(std::shared_ptr<IGraphicsRenderer>);

    void update() override;
};

#endif // GRAPHICSENGINE_H

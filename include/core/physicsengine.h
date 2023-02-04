#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include <memory>

#include <core/forwarddecl.h>

class PhysicsEngine : public IEngine
{
public:
    PhysicsEngine(std::shared_ptr<IPhysicsRenderer>);

    void update() override;
};

#endif // PHYSICSENGINE_H

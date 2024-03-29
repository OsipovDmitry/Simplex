#ifndef TESTAPPLICATION_H
#define TESTAPPLICATION_H

#include <vector>

#include <core/applicationbase.h>

class TestApplication : public simplex::core::ApplicationBase
{
public:
    TestApplication(std::shared_ptr<simplex::core::graphics::IRenderer>);

    std::shared_ptr<simplex::core::GraphicsEngine> graphicsEngine();

protected:
    void doUpdate(uint64_t, uint32_t) override;

};

#endif // TESTAPPLICATION_H

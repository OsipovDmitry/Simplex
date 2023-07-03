#ifndef TESTAPPLICATION_H
#define TESTAPPLICATION_H

#include <vector>

#include <core/applicationbase.h>

class TestApplication : public simplex::core::ApplicationBase
{
public:
    TestApplication(std::shared_ptr<simplex::core::IGraphicsRenderer>);

    std::shared_ptr<simplex::core::GraphicsEngine> graphicsEngine();

protected:
    void doUpdate(uint64_t, uint32_t) override;

private:
    std::shared_ptr<simplex::core::GraphicsEngine> m_graphicsEngine;

    std::shared_ptr<simplex::core::Scene> m_scene0, m_scene1;
};

#endif // TESTAPPLICATION_H

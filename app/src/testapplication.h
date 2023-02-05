#ifndef TESTAPPLICATION_H
#define TESTAPPLICATION_H

#include <core/applicationbase.h>

class TestApplication : public simplex::core::ApplicationBase
{
public:
    TestApplication(std::shared_ptr<simplex::core::IGraphicsRenderer>);

private:
    std::shared_ptr<simplex::core::GraphicsEngine> m_graphicsEngine;
};

#endif // TESTAPPLICATION_H

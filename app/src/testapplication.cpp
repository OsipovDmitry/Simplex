#include <core/graphicsengine.h>

#include "testapplication.h"

TestApplication::TestApplication(std::shared_ptr<simplex::core::IGraphicsRenderer> graphicsRenderer)
    : simplex::core::ApplicationBase("TestApplication")
    , m_graphicsEngine(std::make_shared<simplex::core::GraphicsEngine>("Simplex3DGraphicsEngine", graphicsRenderer))
{
    registerEngine(m_graphicsEngine);
}

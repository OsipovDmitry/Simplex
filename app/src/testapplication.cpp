#include <core/graphicsengine.h>

#include "testapplication.h"

TestApplication::TestApplication(std::shared_ptr<simplex::core::IGraphicsRenderer> graphicsRenderer)
    : simplex::core::ApplicationBase()
    , m_graphicsEngine(std::make_shared<simplex::core::GraphicsEngine>(graphicsRenderer))
{
    registerEngine(m_graphicsEngine);
}

#include <utils/logger.h>

#include <core/graphicsengine.h>

#include "graphicsengineprivate.h"

namespace simplex
{
namespace core
{

GraphicsEngine::GraphicsEngine(const std::string &name, std::shared_ptr<IGraphicsRenderer> renderer)
    : m_(std::make_unique<GraphicsEnginePrivate>(name, renderer))
{
    LOG_INFO << "Engine \"" << m_->name() << "\" has been created";
}

GraphicsEngine::~GraphicsEngine()
{
    LOG_INFO << "Engine \"" << m_->name() << "\" has been destroyed";
}

std::string GraphicsEngine::name() const
{
    return m_->name();
}

void GraphicsEngine::update()
{

}

}
}

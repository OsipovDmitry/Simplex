#include <utils/logger.h>

#include "programsloaderprivate.h"

namespace simplex
{
namespace core
{

ProgramsLoaderPrivate::ProgramsLoaderPrivate(const std::shared_ptr<graphics::RendererBase> &renderer)
    : m_renderer(renderer)
{
}

ProgramsLoaderPrivate::~ProgramsLoaderPrivate() = default;

std::shared_ptr<graphics::RendererBase> &ProgramsLoaderPrivate::renderer()
{
    return m_renderer;
}

std::unordered_map<std::string, std::shared_ptr<graphics::IProgram>> &ProgramsLoaderPrivate::resources()
{
    return m_resources;
}

}
}

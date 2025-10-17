#include <utils/logger.h>

#include "programsmanagerprivate.h"

namespace simplex
{
namespace core
{

ProgramsManagerPrivate::ProgramsManagerPrivate(const std::shared_ptr<graphics::RendererBase> &renderer)
    : m_renderer(renderer)
{
}

ProgramsManagerPrivate::~ProgramsManagerPrivate() = default;

std::shared_ptr<graphics::RendererBase> &ProgramsManagerPrivate::renderer()
{
    return m_renderer;
}

std::unordered_map<std::string, std::shared_ptr<graphics::IProgram>> &ProgramsManagerPrivate::resources()
{
    return m_resources;
}

}
}

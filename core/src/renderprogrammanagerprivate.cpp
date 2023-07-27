#include "renderprogramsmanagerprivate.h"

namespace simplex
{
namespace core
{

RenderProgramsManagerPrivate::RenderProgramsManagerPrivate(std::shared_ptr<graphics::IRenderer> renderer)
    : m_renderer(renderer)
{
}

RenderProgramsManagerPrivate::~RenderProgramsManagerPrivate()
{
}

std::shared_ptr<graphics::IRenderer> &RenderProgramsManagerPrivate::renderer()
{
    return m_renderer;
}

std::unordered_map<std::string, std::shared_ptr<graphics::IRenderProgram>> &RenderProgramsManagerPrivate::resources()
{
    return m_resources;
}

}
}

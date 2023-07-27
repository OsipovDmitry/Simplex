#ifndef CORE_RENDERPROGRAMSMANAGERPRIVATE_H
#define CORE_RENDERPROGRAMSMANAGERPRIVATE_H

#include <memory>
#include <unordered_map>
#include <string>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class RenderProgramsManagerPrivate
{
public:
    RenderProgramsManagerPrivate(std::shared_ptr<graphics::IRenderer>);
    ~RenderProgramsManagerPrivate();

    std::shared_ptr<graphics::IRenderer> &renderer();
    std::unordered_map<std::string, std::shared_ptr<graphics::IRenderProgram>> &resources();

private:
    std::shared_ptr<graphics::IRenderer> m_renderer;
    std::unordered_map<std::string, std::shared_ptr<graphics::IRenderProgram>> m_resources;
};

}
}

#endif // CORE_RENDERPROGRAMSMANAGERPRIVATE_H

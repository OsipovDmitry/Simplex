#ifndef CORE_PROGRAMSMANAGERPRIVATE_H
#define CORE_PROGRAMSMANAGERPRIVATE_H

#include <memory>
#include <unordered_map>
#include <string>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class ProgramsManagerPrivate
{
public:
    ProgramsManagerPrivate(const std::shared_ptr<graphics::RendererBase>&);
    ~ProgramsManagerPrivate();

    std::shared_ptr<graphics::RendererBase> &renderer();
    std::unordered_map<std::string, std::shared_ptr<graphics::IProgram>> &resources();

private:
    std::shared_ptr<graphics::RendererBase> m_renderer;
    std::unordered_map<std::string, std::shared_ptr<graphics::IProgram>> m_resources;
};

}
}

#endif // CORE_PROGRAMSMANAGERPRIVATE_H

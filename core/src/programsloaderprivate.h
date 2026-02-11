#ifndef CORE_PROGRAMSLOADERPRIVATE_H
#define CORE_PROGRAMSLOADERPRIVATE_H

#include <memory>
#include <unordered_map>
#include <string>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class ProgramsLoaderPrivate
{
public:
    ProgramsLoaderPrivate(const std::shared_ptr<graphics::RendererBase>&);
    ~ProgramsLoaderPrivate();

    std::shared_ptr<graphics::RendererBase> &renderer();
    std::unordered_map<std::string, std::shared_ptr<graphics::IProgram>> &resources();

private:
    std::shared_ptr<graphics::RendererBase> m_renderer;
    std::unordered_map<std::string, std::shared_ptr<graphics::IProgram>> m_resources;
};

}
}

#endif // CORE_PROGRAMSLOADERPRIVATE_H

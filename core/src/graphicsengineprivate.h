#ifndef GRAPHICSENGINEPRIVATE_H
#define GRAPHICSENGINEPRIVATE_H

#include <string>
#include <memory>
#include <vector>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class GraphicsEnginePrivate
{
public:
    GraphicsEnginePrivate(const std::string &name, std::shared_ptr<IGraphicsRenderer> renderer)
        : m_name(name)
        , m_renderer(renderer)
    {}

    std::string &name() { return m_name; }
    std::shared_ptr<IGraphicsRenderer> &renderer() { return m_renderer; }
    std::vector<std::shared_ptr<Scene>> &scenes() { return m_scenes; }

private:
    std::string m_name;
    std::shared_ptr<IGraphicsRenderer> m_renderer;
    std::vector<std::shared_ptr<Scene>> m_scenes;
};

}
}

#endif // GRAPHICSENGINEPRIVATE_H

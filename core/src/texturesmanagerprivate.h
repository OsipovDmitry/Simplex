#ifndef CORE_TEXTURESMANAGERPRIVATE_H
#define CORE_TEXTURESMANAGERPRIVATE_H

#include <memory>
#include <unordered_map>
#include <string>

#include <utils/forwarddecl.h>
#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class TexturesManagerPrivate
{
public:
    TexturesManagerPrivate(std::shared_ptr<graphics::IRenderer>);
    ~TexturesManagerPrivate();

    std::shared_ptr<graphics::IRenderer> &renderer();
    std::unordered_map<std::string, std::shared_ptr<graphics::ITexture>> &resources();

private:
    std::shared_ptr<graphics::IRenderer> m_renderer;
    std::unordered_map<std::string, std::shared_ptr<graphics::ITexture>> m_resources;
};

}
}

#endif // CORE_TEXTURESMANAGERPRIVATE_H

#ifndef CORE_TEXTURESLOADERPRIVATE_H
#define CORE_TEXTURESLOADERPRIVATE_H

#include <memory>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class TexturesLoaderPrivate
{
public:
    TexturesLoaderPrivate(const std::shared_ptr<graphics::RendererBase>&);
    ~TexturesLoaderPrivate();

    std::shared_ptr<graphics::RendererBase> &renderer();

private:
    std::shared_ptr<graphics::RendererBase> m_renderer;
};

}
}

#endif // CORE_TEXTURESLOADERPRIVATE_H

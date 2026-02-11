#include "texturesloaderprivate.h"

namespace simplex
{
namespace core
{

TexturesLoaderPrivate::TexturesLoaderPrivate(const std::shared_ptr<graphics::RendererBase>& renderer)
    : m_renderer(renderer)
{
}

TexturesLoaderPrivate::~TexturesLoaderPrivate() = default;

std::shared_ptr<graphics::RendererBase> &TexturesLoaderPrivate::renderer()
{
    return m_renderer;
}

}
}

#include "soundsloaderprivate.h"

namespace simplex
{
namespace core
{

SoundsLoaderPrivate::SoundsLoaderPrivate(const std::shared_ptr<audio::RendererBase> &renderer)
    : m_renderer(renderer)
{
}

SoundsLoaderPrivate::~SoundsLoaderPrivate() = default;

std::shared_ptr<audio::RendererBase> &SoundsLoaderPrivate::renderer()
{
    return m_renderer;
}

}
}

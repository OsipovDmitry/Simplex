#include <scenegraph/drawable.h>

#include <types/geometry.h>

#include "drawable_p.h"

namespace scenegraph
{

renderer::ContextPtr Drawable::context() const
{
    return m_->context;
}

renderer::VertexArrayPtr Drawable::vertexArray() const
{
    return m_->vao;
}

void Drawable::setVertexArray(renderer::VertexArrayPtr value)
{
    m_->vao = value;
}

DrawablePtr Drawable::create(renderer::ContextPtr context)
{
    auto drawable = DrawablePtr(new Drawable(context), DrawableDeleter());
    return drawable;
}

Drawable::Drawable(renderer::ContextPtr context) :
    m_(new DrawablePrivate(context))
{
}

Drawable::~Drawable()
{
    delete m_;
}



}

#include <scenegraph/drawable.h>
#include <scenegraph/renderstate.h>

#include <types/geometry.h>

#include "drawable_p.h"

namespace scenegraph
{

void Drawable::setGeometry(renderer::VertexArrayPtr vao, types::PrimitiveType primitiveType, uint32_t numIndices, types::IndexType indicesType, uint32_t bufferOffset)
{
    m_->vao = vao;
    m_->primitivesType = primitiveType;
    m_->numIndices = numIndices;
    m_->indicesType = indicesType;
    m_->indexBufferOffset = bufferOffset;
}

renderer::VertexArrayPtr Drawable::vertexArray() const
{
    return m_->vao;
}

types::PrimitiveType Drawable::primitiveType() const
{
    return m_->primitivesType;
}

uint32_t Drawable::numIndices() const
{
    return m_->numIndices;
}

types::IndexType Drawable::indicesType() const
{
    return m_->indicesType;
}

uint32_t Drawable::indexBufferOffset() const
{
    return m_->indexBufferOffset;
}

MaterialPtr Drawable::material() const
{
    return m_->material;
}

void Drawable::setMaterial(MaterialPtr value)
{
    m_->material = value;
}

const RenderState &Drawable::renderState() const
{
    return *m_->renderState;
}

RenderState &Drawable::renderState()
{
    return *m_->renderState;
}

DrawablePtr Drawable::create()
{
    auto drawable = DrawablePtr(new Drawable(), DrawableDeleter());
    return drawable;
}

Drawable::Drawable() :
    m_(new DrawablePrivate())
{
}

Drawable::~Drawable()
{
    delete m_;
}



}

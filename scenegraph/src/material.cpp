#include <scenegraph/material.h>

#include "material_p.h"

namespace scenegraph
{

MaterialPtr Material::create(renderer::ProgramPtr program)
{
    auto material = MaterialPtr(new Material(program), MaterialDeleter());
    return material;
}

Material::Material(renderer::ProgramPtr program) :
    m_(new MaterialPrivate(program))
{

}

Material::~Material()
{
    delete m_;
}



}

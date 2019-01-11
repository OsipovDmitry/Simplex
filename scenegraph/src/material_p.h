#ifndef MATERIAL_P_H
#define MATERIAL_P_H

#include <renderer/forwarddecl.h>

namespace scenegraph
{

class MaterialPrivate
{
public:
    renderer::ProgramPtr program;

    MaterialPrivate(renderer::ProgramPtr p) : program(p) {}
};

}

#endif // MATERIAL_P_H

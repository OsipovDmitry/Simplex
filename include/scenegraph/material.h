#ifndef MATERIAL_H
#define MATERIAL_H

#include "scenegraph_global.h"
#include "forwarddecl.h"
#include "../renderer/forwarddecl.h"
#include "../utils/pimpl.h"
#include "../utils/noncopyble.h"
#include "../utils/customdeleter.h"

namespace scenegraph
{

class MaterialPrivate;
class SCENEGRAPHSHARED_EXPORT Material {
    PIMPL(Material)
    NONCOPYBLE(Material)
    CUSTOMDELETER(Material)

public:
    static MaterialPtr create(renderer::ProgramPtr program);

private:
    Material(renderer::ProgramPtr program);
    ~Material();

    MaterialPrivate *m_;
};

}

#endif // MATERIAL_H

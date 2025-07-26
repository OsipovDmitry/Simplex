#ifndef CORE_SCREENQUAD_H
#define CORE_SCREENQUAD_H

#include <utils/singleton.h>

#include <core/mesh.h>

namespace simplex
{
namespace core
{

class ScreenQuad : public Mesh
{
    SINGLETON_POINTER(ScreenQuad)
public:
    ~ScreenQuad() override;

private:
    ScreenQuad();

};

}
}

#endif // CORE_SCREENQUAD_H

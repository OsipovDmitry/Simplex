#ifndef CORE_SCREENQUADDRAWABLE_H
#define CORE_SCREENQUADDRAWABLE_H

#include <utils/singleton.h>

#include <core/mesh.h>

//tmp
#include <core/drawable.h>

namespace simplex
{
namespace core
{

class ScreenQuadDrawable : public Mesh  /*tmp*/, public Drawable
{
    SINGLETON_POINTER(ScreenQuadDrawable)
public:
    ~ScreenQuadDrawable() override;

private:
    ScreenQuadDrawable();

};

}
}

#endif // CORE_SCREENQUADDRAWABLE_H

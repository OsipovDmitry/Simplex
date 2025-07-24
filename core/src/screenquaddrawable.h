#ifndef CORE_SCREENQUADDRAWABLE_H
#define CORE_SCREENQUADDRAWABLE_H

#include <core/drawable.h>

namespace simplex
{
namespace core
{

class ScreenQuadDrawable : public Drawable
{
public:
    ScreenQuadDrawable();
    ~ScreenQuadDrawable() override;

    static std::shared_ptr<const Mesh> mesh();

};

}
}

#endif // CORE_SCREENQUADDRAWABLE_H

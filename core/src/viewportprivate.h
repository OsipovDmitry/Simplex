#ifndef CORE_VIEWPORTPRIVATE_H
#define CORE_VIEWPORTPRIVATE_H

#include <core/viewport.h>

namespace simplex
{
namespace core
{

class ViewportPrivate
{
public:
    ViewportPrivate();
    ~ViewportPrivate();

    Viewport::SizePolicy &sizePolicy();
    glm::uvec4 &size();

private:
    Viewport::SizePolicy m_policy;
    glm::uvec4 m_size;
};

}
}

#endif // CORE_VIEWPORTPRIVATE_H

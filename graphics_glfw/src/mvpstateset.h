#ifndef QT_MVPSTATESET_H
#define QT_MVPSTATESET_H

#include <utils/glm/mat4x4.hpp>

#include <core/stateset.h>

namespace simplex
{
namespace graphics_glfw
{

class MVPStateSet : public core::StateSet
{
public:
    MVPStateSet();
    ~MVPStateSet() override;

    void setModelMatrix(const glm::mat4x4&);

};

}
}

#endif // QT_MVPSTATESET_H

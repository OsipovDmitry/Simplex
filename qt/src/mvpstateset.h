#ifndef QT_MVPSTATESET_H
#define QT_MVPSTATESET_H

#include <utils/glm/mat4x4.hpp>

#include <core/stateset.h>

namespace simplex
{
namespace qt
{

class MVPStateSet : public core::StateSet
{
public:
    MVPStateSet();
    ~MVPStateSet() override;

    void setViewportSize(const glm::uvec2&);

    void setModelMatrix(const glm::mat4x4&);
    void setViewMatrix(const glm::mat4x4&);
    void setProjectionMatrix(const glm::mat4x4&);

private:
    void setModelViewMatrix(const glm::mat4x4&, const glm::mat4x4&);
    void setViewProjectionMatrix(const glm::mat4x4&, const glm::mat4x4&);
    void setModelViewProjectionMatrix(const glm::mat4x4&, const glm::mat4x4&, const glm::mat4x4&);

};

}
}

#endif // QT_MVPSTATESET_H

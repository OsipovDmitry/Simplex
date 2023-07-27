#ifndef CORE_STANDARDDRAWABLEPRIVATE_H
#define CORE_STANDARDDRAWABLEPRIVATE_H

#include <utils/glm/vec4.hpp>

#include <core/idrawable.h>

#include "drawablebaseprivate.h"

namespace simplex
{
namespace core
{

class StandardDrawablePrivate : public DrawableBasePrivate
{
public:
    StandardDrawablePrivate(std::shared_ptr<graphics::IRenderProgram>, std::shared_ptr<graphics::IVertexArray>);

    int32_t &baseColorUniformLocation();
    int32_t &metalnessUniformLocation();
    int32_t &roughnessUniformLocation();
    int32_t &baseColorMapUniformLocation();
    int32_t &metalnessMapUniformLocation();
    int32_t &roughnessMapUniformLocation();
    int32_t &normalMapUniformLocation();

protected:
    int32_t m_baseColorUniformLocation;
    int32_t m_metalnessUniformLocation;
    int32_t m_roughnessUniformLocation;
    int32_t m_baseColorMapUniformLocation;
    int32_t m_metalnessMapUniformLocation;
    int32_t m_roughnessMapUniformLocation;
    int32_t m_normalMapUniformLocation;

};

}
}

#endif // CORE_STANDARDDRAWABLEPRIVATE_H

#include "standarddrawableprivate.h"

namespace simplex
{
namespace core
{

StandardDrawablePrivate::StandardDrawablePrivate(std::shared_ptr<graphics::IRenderProgram> renderProgram,
                                                 std::shared_ptr<graphics::IVertexArray> vao)
    : DrawableBasePrivate(renderProgram, vao)
    , m_baseColorUniformLocation(-1)
    , m_metalnessUniformLocation(-1)
    , m_roughnessUniformLocation(-1)
    , m_baseColorMapUniformLocation(-1)
    , m_metalnessMapUniformLocation(-1)
    , m_roughnessMapUniformLocation(-1)
    , m_normalMapUniformLocation(-1)
{
}

int32_t &StandardDrawablePrivate::baseColorUniformLocation()
{
    return m_baseColorUniformLocation;
}

int32_t &StandardDrawablePrivate::metalnessUniformLocation()
{
    return m_metalnessUniformLocation;
}

int32_t &StandardDrawablePrivate::roughnessUniformLocation()
{
    return m_roughnessUniformLocation;
}

int32_t &StandardDrawablePrivate::baseColorMapUniformLocation()
{
    return m_baseColorMapUniformLocation;
}

int32_t &StandardDrawablePrivate::metalnessMapUniformLocation()
{
    return m_metalnessMapUniformLocation;
}

int32_t &StandardDrawablePrivate::roughnessMapUniformLocation()
{
    return m_roughnessMapUniformLocation;
}

int32_t &StandardDrawablePrivate::normalMapUniformLocation()
{
    return m_normalMapUniformLocation;
}

}
}

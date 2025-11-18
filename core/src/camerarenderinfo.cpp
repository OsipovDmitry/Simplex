#include <utils/glm/gtc/matrix_inverse.hpp>
#include <utils/frustum.h>

#include <core/uniform.h>

#include "descriptions.h"
#include "camerarenderinfo.h"
#include "geometrybuffer.h"

namespace simplex
{
namespace core
{

CameraRenderInfo::CameraRenderInfo()
    : StateSet()
{
    m_cameraBuffer = PCameraBuffer::element_type::create();
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CameraBuffer) = graphics::BufferRange::create(m_cameraBuffer->buffer());
}

CameraRenderInfo::~CameraRenderInfo() = default;

void CameraRenderInfo::setFrustum(
    const utils::Transform& viewTransform,
    const utils::ClipSpace& clipSpace,
    const utils::Range& ZRange)
{
    m_cameraBuffer->set(CameraDescription::make(utils::Frustum(viewTransform, clipSpace, ZRange)));
}

}
}

#ifndef CORE_GEOMETRYRENDERINFO_H
#define CORE_GEOMETRYRENDERINFO_H

#include <utils/transform.h>
#include <utils/clipspace.h>

#include <core/forwarddecl.h>
#include <core/stateset.h>

namespace simplex
{
namespace core
{

struct CameraDescription;
using PCameraBuffer = std::shared_ptr<graphics::StructBuffer<CameraDescription>>;

class CameraRenderInfo : public StateSet
{
public:
    CameraRenderInfo();
    ~CameraRenderInfo() override;

    void setFrustum(const utils::Transform&, const utils::ClipSpace&, const utils::Range&);

private:
    PCameraBuffer m_cameraBuffer;
};

}
}

#endif // CORE_GEOMETRYRENDERINFO_H
